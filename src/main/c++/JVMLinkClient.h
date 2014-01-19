/*
 * #%L
 * JVMLink client/server architecture for communicating between Java and
 * non-Java programs using sockets.
 * %%
 * Copyright (C) 2008 - 2014 Board of Regents of the University of
 * Wisconsin-Madison.
 * %%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * #L%
 */

#ifndef JVMLINKCLIENT_HH
#define JVMLINKCLIENT_HH

#include "JVMLinkObject.h"

#include <string>

// NB: Toggle comments to control debugging output for the client.
#define debug(msg) ((void)0)
//#define debug(msg) std::cout << "JVMLinkClient: " << msg << std::endl

class JVMException : public std::exception
{
public:
	JVMException(const std::string& aMessage) throw ();
	virtual ~JVMException() throw ();

	virtual const char* what() const throw();

private:
	std::string mMessage;
};


class JVMLinkClient
{
private:
	unsigned short port;
#ifdef _WIN32
	SOCKET conn;
#else
	int conn;
#endif

	void sendMessage(const std::string&);
	void* readMessage(int);
	void sendInt(int);
	int readInt();
	std::string* readString();

public:
	enum ConnectionCode {
		CONNECTION_SUCCESS = 0,
		WINSOCK_ERR,
		SOCKET_ERR,
		RESOLVE_ERR,
		RESPONSE_ERR
	};

	JVMLinkClient();
	void startJava(unsigned short, const std::string&);
	void shutJava();
	ConnectionCode establishConnection();
	int closeConnection();
	JVMLinkObject* getVar(const std::string&);
	void setVar(const JVMLinkObject&);
	void setVar(const std::string&, int);
	void setVar(const std::string&, int*, int);
	void setVar(const std::string&, std::string*);
	void setVar(const std::string&, std::string*, int);
	void setVar(const std::string&, char);
	void setVar(const std::string&, char*, int);
	void setVar(const std::string&, Byte);
	void setVar(const std::string&, Byte*, int);
	void setVar(const std::string&, float);
	void setVar(const std::string&, float*, int);
	void setVar(const std::string&, bool);
	void setVar(const std::string&, bool*, int);
	void setVar(const std::string&, double);
	void setVar(const std::string&, double*, int);
	void setVar(const std::string&, long long);
	void setVar(const std::string&, long long*, int);
	void setVar(const std::string&, short);
	void setVar(const std::string&, short*, int);
	void setVarNull(const std::string&);
	void exec(const std::string&);

	~JVMLinkClient(void);
};

#endif // JVMLINKCLIENT_HH
