//
// JVMLinkClient.cpp
//

/*
JVMLink client/server architecture for communicating between Java and
non-Java programs using sockets.
Copyright (c) 2008 Hidayath Ansari and Curtis Rueden. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the UW-Madison LOCI nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE UW-MADISON LOCI ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef _WIN32
#include "stdafx.h"
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include "JVMLinkClient.h"

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>

#define DEFAULT_PORT 20345

//TODO: clear memory at appropriate points.

JVMException::JVMException(const std::string& aMessage) throw()
{
	mMessage = aMessage;
}

JVMException::~JVMException() throw()
{
}

const char* JVMException::what() const throw()
{
	return mMessage.c_str();
}


JVMLinkClient::JVMLinkClient(void)
{
}

JVMLinkClient::~JVMLinkClient(void)
{
}

// -- Public API methods --

void JVMLinkClient::startJava(unsigned short arg_port, const std::string& classpath) {
	port = arg_port == 0 ? DEFAULT_PORT : arg_port;
	std::stringstream tmpportstr;
	tmpportstr << port;
	const std::string portstr = tmpportstr.str();

#ifdef _WIN32
	// NB: Toggle comments to control debugging output for the server.
	const std::string command = "-cp " + classpath + " loci.jvmlink.JVMLinkServer " + portstr;
	//const std::string command = "-cp " + classpath + " loci.jvmlink.JVMLinkServer -debug " + portstr;
	debug("java " << command);
	ShellExecute(NULL, "open", "javaw.exe" , command, "", SW_SHOW);
	//ShellExecute(NULL, "open", "java.exe" , command, "", SW_SHOW);
#else
	pid_t vProcID = vfork();
	if (vProcID == 0) {
		// executed by child process
		execlp("java", "java", "-cp", classpath.c_str(), "loci.jvmlink.JVMLinkServer", portstr.c_str(), "-debug", (char*)NULL);
		//execlp("java", "java", "-cp", classpath.c_str(), "loci.jvmlink.JVMLinkServer", portstr.c_str(), (char*)NULL);

		std::cerr << "Error: Child failed to execute process." << std::endl;
		_exit(1);
	} else if (vProcID < 0) {
		// failed to fork
		debug("Error: Failed to fork, will exit now.");
		exit(1);
	} else {
		// Code only executed by parent process
		// TODO: check if the forked child is alive and working
	}
#endif
}

void JVMLinkClient::shutJava() {
	debug("Terminating JVMLink server");
	sendInt(EXIT_CMD);
}

JVMLinkClient::ConnectionCode JVMLinkClient::establishConnection() {
	const std::string servername = "127.0.0.1";

#ifdef _WIN32
	WSADATA wsaData;
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;

	int wsaret=WSAStartup(0x101,&wsaData);
	if (wsaret) return WINSOCK_ERR;
	debug("Initialized WinSock");

	conn=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (conn==INVALID_SOCKET) return SOCKET_ERR;
	debug("Socket created");

	if (inet_addr(servername)==INADDR_NONE) {
		hp=gethostbyname(servername);
	}
	else {
		addr=inet_addr(servername);
		hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
	}
	if (hp == NULL) {
		closesocket(conn);
		debug("Could not resolve network address: " << servername);
		return RESOLVE_ERR;
	}
	debug("Network address resolved");

	server.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	if (connect(conn,(struct sockaddr*)&server,sizeof(server))) {
		closesocket(conn);
		debug("No server response on port " << port);
		return RESPONSE_ERR;
	}
#else
	debug("starting to create socket");
	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;  // fill in my IP for me

	// do the lookup
	std::stringstream tmpportstr;
	tmpportstr << port;
	const std::string portstr = tmpportstr.str();
	getaddrinfo(servername.c_str(), portstr.c_str(), &hints, &res);

	// TODO: should do error-checking on getaddrinfo(), and walk
	// the "res" linked list looking for valid entries instead of just
	// assuming the first one is good
	conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (conn < 0) return SOCKET_ERR;

	debug("finished to create socket");

	if (connect(conn, res->ai_addr, res->ai_addrlen) < 0) {
		close(conn);
		debug("No server response on port " << port);
		return RESPONSE_ERR;
	}
#endif

	debug("Connected to server: " << servername);
	return CONNECTION_SUCCESS;
}

int JVMLinkClient::closeConnection() {
	debug("Closing connection");
#ifdef _WIN32
	shutdown(conn, SD_SEND);
	closesocket(conn);
	debug("Socket closed");
	WSACleanup();
	debug("De-initialized WinSock");
#else
	close(conn);
	conn = 0;
#endif
	return CONNECTION_SUCCESS;
}

JVMLinkObject* JVMLinkClient::getVar(const std::string& name) {
	debug("getVar: requesting " << name);
	JVMLinkObject* obj = new JVMLinkObject(name);
	sendInt(GETVAR_CMD);
	sendMessage(name);
	obj->type = (Type) readInt();
	if (obj->type == ARRAY_TYPE) {
		obj->insideType = (Type) readInt();
		obj->length = readInt();
		if (obj->insideType == STRING_TYPE) {
			std::string* s = new std::string[obj->length];
			for (int i=0; i<obj->length; i++) s[i] = *readString();
			obj->data = s;
		}
		else {
			obj->size = readInt();
			obj->data = readMessage(obj->size * obj->length);
		}
		debug("getVar: got array: length=" << obj->length << ", type=" << obj->insideType);
	}
	else if (obj->type == STRING_TYPE) {
		obj->data = readString();
		obj->size = 0;
		debug("getVar: got string: length=" << len << ", value=" << buf);
	}
	else if (obj->type == NULL_TYPE) {
		obj->data = NULL;
		obj->size = 0;
		debug("getVar: got NULL value");
	}
	else {
		int size = readInt();
		obj->data = readMessage(size);
		obj->size = size;
		obj->insideType = NULL_TYPE;
		debug("getVar: got object: type=" << obj->type << ", size=" << obj->size);
	}
	return obj;
}

void JVMLinkClient::exec(const std::string& command) {
	debug("exec: " << command);
	sendInt(EXEC_CMD);
	sendMessage(command);
	int status = readInt();
	if (status != 0) {
		std::string vMessage = *readString();
		JVMException vJVMException(vMessage);
		throw vJVMException;
	}
}

void JVMLinkClient::setVar(JVMLinkObject* obj) {
	sendInt(SETVAR_CMD);
	sendMessage(obj->name);
	sendInt((int) obj->type);
	if (obj->type == ARRAY_TYPE) {
		sendInt((int) obj->insideType);
		sendInt(obj->length);
		if (obj->insideType == STRING_TYPE) {
			std::string* s = (std::string*) obj->data;
			for (int i=0; i<obj->length; i++) {
				sendMessage(s[i]);
			}
		}
		else {
			int sent = 0;
			int total = obj->size * obj->length;
			char* buf = (char*) obj->data;
			while (sent < total) {
				sent += send(conn, buf + sent, total - sent, 0);
			}
		}
	}
	else {
		if (obj->type == STRING_TYPE) sendMessage(*(std::string*) obj->data);
		else send(conn, (char*) obj->data, obj->size, 0);
	}
}

void JVMLinkClient::setVar(const std::string& argname, int obj) {
	debug("setVar: " << argname << " = " << obj << " (int)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, INT_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, int* obj, int length) {
	debug("setVar: " << argname << " (int array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, INT_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, std::string* obj) {
	debug("setVar: " << argname << " = " << obj << " (string)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, STRING_TYPE, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, std::string* obj, int length) {
	debug("setVar: " << argname << " (string array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, STRING_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, char obj) {
	debug("setVar: " << argname << " = " << obj << " (char)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, CHAR_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, char* obj, int length) {
	debug("setVar: " << argname << " (char array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, CHAR_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, Byte obj) {
	debug("setVar: " << argname << " = " << obj.data << " (byte)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, BYTE_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, Byte* obj, int length) {
	debug("setVar: " << argname << " (byte array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, BYTE_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, float obj) {
	debug("setVar: " << argname << " = " << obj << " (float)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, FLOAT_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, float* obj, int length) {
	debug("setVar: " << argname << " (float array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, FLOAT_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, bool obj) {
	debug("setVar: " << argname << " = " << obj << " (bool)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, BOOL_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, bool* obj, int length) {
	debug("setVar: " << argname << " (bool array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, BOOL_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, double obj) {
	debug("setVar: " << argname << " = " << obj << " (double)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, DOUBLE_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, double* obj, int length) {
	debug("setVar: " << argname << " (double array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, DOUBLE_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, long long obj) {
	debug("setVar: " << argname << " = " << obj << " (long)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, LONG_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, long long* obj, int length) {
	debug("setVar: " << argname << " (long array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, LONG_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, short obj) {
	debug("setVar: " << argname << " = " << obj << " (short)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, SHORT_TYPE, &obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVar(const std::string& argname, short* obj, int length) {
	debug("setVar: " << argname << " (short array)");
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, SHORT_TYPE, length, obj);
	setVar(jvmObj);
	delete jvmObj;
}

void JVMLinkClient::setVarNull(const std::string& argname) {
	debug("setVarNull: " << argname);
	JVMLinkObject* jvmObj = new JVMLinkObject(argname, NULL_TYPE, NULL);
	setVar(jvmObj);
	delete jvmObj;
}

// -- Private methods --

void JVMLinkClient::sendMessage(const std::string& message) {
	int sent = 0;
	const char* buf = message.c_str();
	int total = message.length();
	sendInt(total);
	while (sent < total) sent += send(conn, buf + sent, total - sent, 0);
}

void JVMLinkClient::sendInt(int value) {
	char* buf = (char*) (&value);
	send(conn, buf, 4, 0);
}

void* JVMLinkClient::readMessage(int size) {
	int read = 0;
	char* buf = (char*) malloc(size);
	while (read < size) read += recv(conn, buf + read, size - read, 0);
	return (void*) buf;
}

int JVMLinkClient::readInt() {
	return *(int*) readMessage(4);
}

std::string* JVMLinkClient::readString() {
	int read = 0;
	int total = readInt();
	char* buf = new char[total + 1];
	while (read < total) read += recv(conn, buf + read, total - read, 0);
	buf[total] = '\0';
	return new std::string(buf);
}
