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

#ifndef JVMLINKOBJECT_HH
#define JVMLINKOBJECT_HH

#include <string>
#include <vector>

enum Command {
	BYTE_ORDER_CMD = 0,
	SETVAR_CMD = 1,
	GETVAR_CMD = 2,
	EXEC_CMD = 3,
	EXIT_CMD = 255
};

enum Type {
	ARRAY_TYPE = 0,
	INT_TYPE,
	STRING_TYPE,
	BYTE_TYPE,
	CHAR_TYPE,
	FLOAT_TYPE,
	BOOL_TYPE,
	DOUBLE_TYPE,
	LONG_TYPE,
	SHORT_TYPE,
	NULL_TYPE = -1
};

struct Byte {
	char data;
};

class JVMLinkObject {
private:
	int getSize(Type);

public:
	JVMLinkObject(const std::string&);
	JVMLinkObject(const std::string&, Type, const void*);
	JVMLinkObject(const std::string&, Type, int, const void*);

	~JVMLinkObject(void);

	std::string name;
	int size, length;
	Type type, insideType;
	const void* data;

	int getDataAsInt();
	int* getDataAsIntArray();
	std::string* getDataAsString();
	std::string* getDataAsStringArray();
	char getDataAsChar();
	char* getDataAsCharArray();
	Byte getDataAsByte();
	Byte* getDataAsByteArray();
	float getDataAsFloat();
	float* getDataAsFloatArray();
	bool getDataAsBool();
	bool* getDataAsBoolArray();
	double getDataAsDouble();
	double* getDataAsDoubleArray();
	long long getDataAsLong();
	long long* getDataAsLongArray();
	short getDataAsShort();
	short* getDataAsShortArray();
	bool isDataNull();
};

#endif // JVMLINKOBJECT_HH
