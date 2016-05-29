#include <string>
#include <cstring>
#include <iostream>
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

int main( int argc, char *argv[] ) {

	string a = "01235007helloworld";
	char *str;
	str = new char [a.length()];
	strcpy(str, a.c_str());
	//cout << endl << str << endl;
	TCPDatagramBuilder b;
	b.feed(str);
	TCPDatagram o = *(b.getDatagram());
	cout << "Sequence Num: " << o.sequenceNum << endl;
	cout << endl << "ACK Number: " << o.ackNum << endl;
	cout << endl << "Window Size: " << o.windowSize << endl;
	cout << endl << "Data: " << o.data << endl;
	cout << endl << "Excess: " << b.currentString << endl;

	delete str;
	return 0;
}
