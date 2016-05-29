#include <string>
#include <cstring>
#include <iostream>
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

const int BUFFER_SIZE = 5;

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
	str = new char[a.length()];
	strcpy(str, a.c_str());

	TCPDatagramBuilder b;


	// split into parts to simulate streaming data
	char *buf;
	buf = new char [BUFFER_SIZE+1]; // extra character for null terminator
	for (unsigned int i = 0; i < strlen(str); i+=BUFFER_SIZE) {
		memset(buf, '\0', BUFFER_SIZE+1); // clear buffer
		strncpy(buf, str+i, BUFFER_SIZE);
		// feed into datagram builder
		b.feed(buf);
	}

	TCPDatagram o = *(b.getDatagram());
	cout << "Sequence Num: " << o.sequenceNum << endl;
	cout << "ACK Number: " << o.ackNum << endl;
	cout << "Window Size: " << o.windowSize << endl;
	cout << "Data: " << o.data << endl;
	cout << "ACK: " << o.ACK << endl;
	cout << "SYN: " << o.SYN << endl;
	cout << "FIN: " << o.FIN << endl;
	cout << "Datagram complete? " << (b.isComplete() ? "true" : "false") << endl;
	cout << "Excess: " << b.currentString << endl;

	delete str;
	return 0;
}
