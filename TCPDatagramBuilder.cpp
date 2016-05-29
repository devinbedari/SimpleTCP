#include "TCPDatagramBuilder.h"

using namespace std;

// Default constructor
TCPDatagramBuilder::TCPDatagramBuilder() {
	this->datagram = new TCPDatagram();
	this->currentState = SEQ_NUM;
}

// Destructor
TCPDatagramBuilder::~TCPDatagramBuilder() {
}

// Constructor to initialize a base datagram
TCPDatagramBuilder::TCPDatagramBuilder(TCPDatagram* datagram) {
	this->datagram = datagram;
}

// Constructor with an initial string to parse
TCPDatagramBuilder::TCPDatagramBuilder(char* initialString) {
	this->feed(initialString);
}

int stoi (string str) {
	char* temp = new char[str.length()];
	std::strcpy(temp, str.c_str());
	return atoi(temp);
}

unsigned int TCPFieldToUInt (string str) {
	// first reverse the chars, because TCP fields are given in network-byte order, aka Big Endian
	string littleEndian = "";
	for (int i = str.length()-1; i >= 0; i--)
		littleEndian += str.at(i);
	return (unsigned int) stoi(littleEndian);
}

// add buffer to input stream, and process the updated input stream
void TCPDatagramBuilder::feed(char* buffer) {
	this->currentString += buffer;
	this->process();
}

// process input stream
void TCPDatagramBuilder::process()  {

	TCPField before = this->currentState;

	switch (this->currentState) {

		case SEQ_NUM:
			if (this->currentString.length() < 2) break; // incomplete field, can't parse it yet

			this->datagram->sequenceNum = TCPFieldToUInt(this->currentString.substr(0,2));
			this->currentString = this->currentString.substr(2); // finished parsing the field, remove it from the input stream

			this->currentState = ACK_NUM; // change state
			break;

		case ACK_NUM:
			if (this->currentString.length() < 2) break; // incomplete field, can't parse it yet

			this->datagram->ackNum = TCPFieldToUInt(this->currentString.substr(0,2));
			this->currentString = this->currentString.substr(2); // finished parsing the field, remove it from the input stream

			this->currentState = WINDOW_SIZE; // change state
			break;

		case WINDOW_SIZE:
			if (this->currentString.length() < 2) break; // incomplete field, can't parse it yet

			this->datagram->windowSize = TCPFieldToUInt(this->currentString.substr(0,2));
			this->currentString = this->currentString.substr(2); // finished parsing the field, remove it from the input stream

			this->currentState = FLAGS; // change state
			break;

		case FLAGS:
			if (this->currentString.length() < 2) break; // incomplete field, can't parse it yet

			this->datagram->ACK = (this->currentString.at(5) == '1');
			this->datagram->SYN = (this->currentString.at(6) == '1');
			this->datagram->FIN = (this->currentString.at(7) == '1');
			this->currentString = this->currentString.substr(2); // finished parsing the field, remove it from the input stream

			this->currentState = DATA; // change state
			break;

		case DATA:
			// consume all
			this->datagram->data += this->currentString;
			currentString = "";

			if (this->datagram->data.length() >= this->datagram->windowSize) {
				// clip data to window size, give excess back to currentString
				this->currentString = this->datagram->data.substr(this->datagram->windowSize);
				this->datagram->data = this->datagram->data.substr(0, this->datagram->windowSize);

				this->currentState = DONE;
			}
			break;

		case DONE:
			// do nothing, just let currentString grow
			break;

		default:
			cerr << "TCPDatagramBuilder: unknown state" << endl;
			break;
	}

	// if the state changed, we need to run the new state
	if (this->currentState != before) this->process();
}

// Accessors
TCPDatagram* TCPDatagramBuilder::getDatagram() { return this->datagram; }
