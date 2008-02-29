//
// JVMLinkClient.h
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

#pragma once
#include "JVMLinkObject.h"

// NB: Toggle comments to control debugging output for the client.
//#define debug(msg) ((void)0)
#define debug(msg) std::cout << "JVMLinkClient: " << msg << std::endl

class JVMLinkClient
{
private:
	int port;
	SOCKET conn;

	int sendMessage(CString);
	void* readMessage(int);
	int sendInt(int);
	int readInt();

public:
	enum ConnectionCode {
		CONNECTION_SUCCESS = 0,
		WINSOCK_ERR,
		SOCKET_ERR,
		RESOLVE_ERR,
		RESPONSE_ERR
	};

	JVMLinkClient(void);
	void startJava(int, CString);
	void shutJava();
	ConnectionCode establishConnection();
	int closeConnection();
	JVMLinkObject* getVar(CString);
	void setVar(JVMLinkObject*);
	void setVar(CString, int);
	void setVar(CString, CString);
	void setVar(CString, char);
	void setVar(CString, Byte);
	void setVar(CString, float);
	void setVar(CString, bool);
	void setVar(CString, double);
	void setVar(CString, long long);
	void setVar(CString, short);
	void exec(CString);

	~JVMLinkClient(void);
};
