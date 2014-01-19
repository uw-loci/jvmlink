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

#ifdef _WIN32
#include <stdafx.h>
#else
#include <unistd.h> // for usleep()
#endif

#include "JVMLinkClient.h"

#include <ctime> // for time()
#include <cstdlib> // for srand() and rand()
#include <iostream> // for cout and cerr
#include <stdio.h> // for getchar
#include <memory> // for auto_ptr

bool randomBool() {
	return rand() % 2 == 0;
}
Byte randomByte() {
	Byte b;
	b.data = (int) (rand() % 128);
	return b;
}
char randomChar() {
	return (char) (rand() % (127 - 33 + 1)) + 33;
}
double randomDouble() {
	return (double) rand();
}
float randomFloat() {
	return (float) rand();
}
int randomInt() {
	return rand() + 65536;
}
long long randomLong() {
	return (long long) rand() + 0xffffffffL;
}
short randomShort() {
	return (short) rand();
}

template <class T>
void printValues(T* values, int len) {
	for (int i=0; i<len; i++) std::cout << " " << values[i];
}

void printValues(Byte* values, int off, int len) {
	for (int i=off; i<off+len; i++) std::cout << " " << values[i].data;
}

// Tests the JVMLink API.
int main(int argc, char* argv[])
{
	JVMLinkClient *p = new JVMLinkClient();
	p->startJava(20345, "jvmlink.jar");
	while (p->establishConnection() != JVMLinkClient::CONNECTION_SUCCESS) {
#ifdef _WIN32
		Sleep(250);
#else
		usleep(250 * 1000);
#endif
	}

	srand((int) time(0));

	// bool variables
	bool myBool = randomBool();
	p->setVar("myBool", myBool);
	std::cout << "TestC2: setVar: myBool -> " << myBool << std::endl;
	std::auto_ptr<JVMLinkObject> jvmBool(p->getVar("myBool"));
	std::cout << "TestC2: getVar: myBool == " << jvmBool->getDataAsBool() << std::endl;

	// Byte variables
	Byte myByte = randomByte();
	p->setVar("myByte", myByte);
	std::cout << "TestC2: setVar: myByte -> " << myByte.data << std::endl;
	std::auto_ptr<JVMLinkObject> jvmByte(p->getVar("myByte"));
	std::cout << "TestC2: getVar: myByte == " << jvmByte->getDataAsByte().data << std::endl;

	// char variables
	char myChar = randomChar();
	p->setVar("myChar", myChar);
	std::cout << "TestC2: setVar: myChar -> " << myChar << std::endl;
	std::auto_ptr<JVMLinkObject> jvmChar(p->getVar("myChar"));
	std::cout << "TestC2: getVar: myChar == " << jvmChar->getDataAsChar() << std::endl;

	// double variables
	double myDouble = randomDouble();
	p->setVar("myDouble", myDouble);
	std::cout << "TestC2: setVar: myDouble -> " << myDouble << std::endl;
	std::auto_ptr<JVMLinkObject> jvmDouble(p->getVar("myDouble"));
	std::cout << "TestC2: getVar: myDouble == " << jvmDouble->getDataAsDouble() << std::endl;

	// float variables
	float myFloat = randomFloat();
	p->setVar("myFloat", myFloat);
	std::cout << "TestC2: setVar: myFloat -> " << myFloat << std::endl;
	std::auto_ptr<JVMLinkObject> jvmFloat(p->getVar("myFloat"));
	std::cout << "TestC2: getVar: myFloat == " << jvmFloat->getDataAsFloat() << std::endl;

	// int variables
	int myInt = randomInt();
	p->setVar("myInt", myInt);
	std::cout << "TestC2: setVar: myInt -> " << myInt << std::endl;
	std::auto_ptr<JVMLinkObject> jvmInt(p->getVar("myInt"));
	std::cout << "TestC2: getVar: myInt == " << jvmInt->getDataAsInt() <<
		(jvmInt->isDataNull() ? " (NULL)" : " (not NULL)") << std::endl;

	// long variables
	long long myLong = randomLong();
	p->setVar("myLong", myLong);
	std::cout << "TestC2: setVar: myLong -> " << myLong << std::endl;
	std::auto_ptr<JVMLinkObject> jvmLong(p->getVar("myLong"));
	std::cout << "TestC2: getVar: myLong == " << jvmLong->getDataAsLong() << std::endl;

	// short variables
	short myShort = randomShort();
	p->setVar("myShort", myShort);
	std::cout << "TestC2: setVar: myShort -> " << myShort << std::endl;
	std::auto_ptr<JVMLinkObject> jvmShort(p->getVar("myShort"));
	std::cout << "TestC2: getVar: myShort == " << jvmShort->getDataAsShort() << std::endl;

	// std::string variables
	std::string myString = "<<The quick brown fox jumps over the lazy dog.>>";
	p->setVar("myString", &myString);
	std::cout << "TestC2: setVar: myString -> " << myString << std::endl;
	std::auto_ptr<JVMLinkObject> jvmString(p->getVar("myString"));
	std::cout << "TestC2: getVar: myString == " << *jvmString->getDataAsString() << std::endl;

	// null variable
	p->setVarNull("myInt");
	std::cout << "TestC2: setVar: myInt -> NULL" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmNull(p->getVar("myInt"));
	std::cout << "TestC2: getVar: myInt == " <<
		(jvmNull->isDataNull() ? "NULL" : "not NULL") << std::endl;

	// non-existent variable
	std::auto_ptr<JVMLinkObject> jvmNonExist(p->getVar("myNonExist"));
	std::cout << "TestC2: getVar: myNonExist == " <<
		(jvmNonExist->isDataNull() ? "NULL" : "not NULL") << std::endl;

	// arrays
	const int num = 15;
	bool* myBools = new bool[num];
	Byte* myBytes = new Byte[num];
	char* myChars = new char[num];
	double* myDoubles = new double[num];
	float* myFloats = new float[num];
	int* myInts = new int[num];
	long long* myLongs = new long long[num];
	short* myShorts = new short[num];

	for (int i=0; i<num; i++) {
		myBools[i] = randomBool();
		myBytes[i] = randomByte();
		myChars[i] = randomChar();
		myDoubles[i] = randomDouble();
		myFloats[i] = randomFloat();
		myInts[i] = randomInt();
		myLongs[i] = randomLong();
		myShorts[i] = randomShort();
	}

	std::string* myStrings = new std::string[6];
	myStrings[0] = "\"There was an Old Man with a beard,";
	myStrings[1] = "Who said, 'It is just as I feared!";
	myStrings[2] = "Two Owls and a Hen,";
	myStrings[3] = "Four Larks and a Wren,";
	myStrings[4] = "Have all built their nests in my beard!'\"";
	myStrings[5] = "- Edward Lear";

	// bool arrays
	p->setVar("myBools", myBools, num);
	std::cout << "TestC2: setVar: myBools -> [";
	printValues(myBools, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmBools(p->getVar("myBools"));
	std::cout << "TestC2: getVar: myBools == [";
	printValues(jvmBools->getDataAsBoolArray(), num);
	std::cout << " ]" << std::endl;

	// Byte arrays
	p->setVar("myBytes", myBytes, num);
	std::cout << "TestC2: setVar: myBytes -> [";
	printValues(myBytes, 0, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmBytes(p->getVar("myBytes"));
	std::cout << "TestC2: getVar: myBytes == [";
	printValues(jvmBytes->getDataAsByteArray(), 0, num);
	std::cout << " ]" << std::endl;

	// char arrays
	p->setVar("myChars", myChars, num);
	std::cout << "TestC2: setVar: myChars -> [";
	printValues(myChars, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmChars(p->getVar("myChars"));
	std::cout << "TestC2: getVar: myChars == [";
	printValues(jvmChars->getDataAsCharArray(), num);
	std::cout << " ]" << std::endl;

	// double arrays
	p->setVar("myDoubles", myDoubles, num);
	std::cout << "TestC2: setVar: myDoubles -> [";
	printValues(myDoubles, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmDoubles(p->getVar("myDoubles"));
	std::cout << "TestC2: getVar: myDoubles == [";
	printValues(jvmDoubles->getDataAsDoubleArray(), num);
	std::cout << " ]" << std::endl;

	// float arrays
	p->setVar("myFloats", myFloats, num);
	std::cout << "TestC2: setVar: myFloats -> [";
	printValues(myFloats, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmFloats(p->getVar("myFloats"));
	std::cout << "TestC2: getVar: myFloats == [";
	printValues(jvmFloats->getDataAsFloatArray(), num);
	std::cout << " ]" << std::endl;

	// int arrays
	p->setVar("myInts", myInts, num);
	std::cout << "TestC2: setVar: myInts -> [";
	printValues(myInts, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmInts(p->getVar("myInts"));
	std::cout << "TestC2: getVar: myInts == [";
	printValues(jvmInts->getDataAsIntArray(), num);
	std::cout << " ]" << std::endl;

	// long arrays
	p->setVar("myLongs", myLongs, num);
	std::cout << "TestC2: setVar: myLongs -> [";
	printValues(myLongs, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmLongs(p->getVar("myLongs"));
	std::cout << "TestC2: getVar: myLongs == [";
	printValues(jvmLongs->getDataAsLongArray(), num);
	std::cout << " ]" << std::endl;

	// short arrays
	p->setVar("myShorts", myShorts, num);
	std::cout << "TestC2: setVar: myShorts -> [";
	printValues(myShorts, num);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmShorts(p->getVar("myShorts"));
	std::cout << "TestC2: getVar: myShorts == [";
	printValues(jvmShorts->getDataAsShortArray(), num);
	std::cout << " ]" << std::endl;

	// std::string arrays
	p->setVar("myStrings", myStrings, 6);
	std::cout << "TestC2: setVar: myStrings -> [" << std::endl;
	for (int i=0; i<6; i++) {
		std::cout << "\t" << myStrings[i] << std::endl;
	}
	std::cout << "]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmStrings(p->getVar("myStrings"));
	std::cout << "TestC2: getVar: myStrings == [" << std::endl;
	printValues(jvmStrings->getDataAsStringArray(), 6);
	std::cout << "]" << std::endl;

	delete myBools;
	delete myBytes;
	delete myChars;
	delete myDoubles;
	delete myFloats;
	delete myInts;
	delete myShorts;
	delete myLongs;

	// exec commands
	std::cout << "TestC2: trying some exec calls" << std::endl;
	p->exec("import java.awt.BorderLayout");
	p->exec("import javax.swing.JButton");
	p->exec("import javax.swing.JFrame");
	p->exec("import javax.swing.JPanel");
	p->exec("frame = new JFrame(\"My Java Frame\")");
	p->exec("pane = new JPanel()");
	p->exec("frame.setContentPane(pane)");
	p->exec("layout = new BorderLayout()");
	p->exec("pane.setLayout(layout)");
	p->exec("button = new JButton(\"Hello world!\")");
	p->exec("pane.add(button, BorderLayout.CENTER)");
	p->exec("frame.setBounds(100, 100, 500, 400)");
	p->exec("frame.setVisible(true)");

	// large Byte array
	const int big = 10000000;
	Byte* largeBytes = new Byte[big];
	for (int i=0; i<big; i++) largeBytes[i] = randomByte();

	p->setVar("largeBytes", largeBytes, big);
	std::cout << "TestC2: setVar: largeBytes -> [";
	printValues(largeBytes, 0, 3);
	std::cout << " ...";
	printValues(largeBytes, big / 2 - 1, 3);
	std::cout << " ...";
	printValues(largeBytes, big - 3, 3);
	std::cout << " ]" << std::endl;
	std::auto_ptr<JVMLinkObject> jvmLargeBytes(p->getVar("largeBytes"));
	std::cout << "TestC2: getVar: largeBytes == [";
	Byte* largeBytes2 = jvmLargeBytes->getDataAsByteArray();
	printValues(largeBytes2, 0, 3);
	std::cout << " ...";
	printValues(largeBytes2, big / 2 - 1, 3);
	std::cout << " ...";
	printValues(largeBytes2, big - 3, 3);
	std::cout << " ]" << std::endl;

	delete largeBytes;

	std::cout << std::endl << std::endl << "Press enter to shut down the server and exit..." << std::endl;
#ifdef _WIN32
	_fgetchar();
#else
	getchar();
#endif

	// free Java resources
	p->exec("frame.dispose()");

	// order the server to shut down
	p->shutJava();

	// close our connection to the server
	p->closeConnection();

	// free local resources
	delete(p);

	return 0;
}
