#include <string>
#include <cstring>
#include <iostream>
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

const int BUFFER_SIZE = 5;

int main( int argc, char *argv[] ) {

	TCPDatagram input;

	input.sequenceNum = 5;
	input.ackNum = 12;
	input.windowSize = 5;
	input.SYN = true;
	input.data = "hello";

	string raw = input.toString();
	raw += "world"; // give some excess
	cout << "Input Stream: " << raw << endl;

	// convert to c string
	char *str;
	str = new char[raw.length()];
	strcpy(str, raw.c_str());

	TCPDatagramBuilder builder;

	// split into parts to simulate streaming data
	char *buf;
	buf = new char [BUFFER_SIZE]; // extra character for null terminator
	for (unsigned int i = 0; i < strlen(str); i+=BUFFER_SIZE) {
		memset(buf, '\0', BUFFER_SIZE); // clear buffer
		strncpy(buf, str+i, BUFFER_SIZE);
		// feed into datagram builder
		builder.feed(buf, BUFFER_SIZE);
	}

	TCPDatagram output = *(builder.getDatagram());
	cout << "Sequence Num: " << output.sequenceNum << endl;
	cout << "ACK Number: " << output.ackNum << endl;
	cout << "Window Size: " << output.windowSize << endl;
	cout << "Data: " << output.data << endl;
	cout << "ACK: " << output.ACK << endl;
	cout << "SYN: " << output.SYN << endl;
	cout << "FIN: " << output.FIN << endl;
	cout << "Datagram complete? " << (builder.isComplete() ? "true" : "false") << endl;
	cout << "Excess: " << builder.currentString << endl;

	delete str;
	return 0;
}
