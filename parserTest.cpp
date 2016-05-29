#include <string>
#include <cstring>
#include <iostream>
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

int main( int argc, char *argv[] ) {

	TCPDatagram d;

	d.sequenceNum = 5;
	d.ackNum = 12;
	d.windowSize = 5;
	d.SYN = true;
	d.data = "hello";

	string a = d.toString();
	a += "world"; // give some excess
	cout << "Input Stream: " << a << endl;

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
	cout << endl << "ACK: " << o.ACK << endl;
	cout << endl << "SYN: " << o.SYN << endl;
	cout << endl << "FIN: " << o.FIN << endl;
	cout << endl << "Excess: " << b.currentString << endl;

	delete str;
	return 0;
}
