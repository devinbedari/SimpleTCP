#include "TCPDatagramBuilder.h"

using namespace std;

// Default constructor
TCPDatagramBuilder::TCPDatagramBuilder() {
	this->datagram = new TCPDatagram();
	this->currentState = SEQ_NUM;
	this->complete = false;
	this->currentString = "";
}

// Destructor
TCPDatagramBuilder::~TCPDatagramBuilder() {
}

// Constructor to initialize a base datagram
TCPDatagramBuilder::TCPDatagramBuilder(TCPDatagram* datagram) {
	this->datagram = datagram;
}

// Constructor with an initial string to parse, remember to call default constructor
TCPDatagramBuilder::TCPDatagramBuilder(char* initialString, int n) : TCPDatagramBuilder::TCPDatagramBuilder() {
	this->feed(initialString, n);
}

// Constructor with an initial string to parse, remember to call default constructor
TCPDatagramBuilder::TCPDatagramBuilder(string initialString) : TCPDatagramBuilder::TCPDatagramBuilder() {
	this->feed(initialString);
}

uint16_t TCPFieldToUInt (string str) {
	uint16_t networkOrder = 0;
	memcpy(&networkOrder, str.c_str(), sizeof(uint16_t));
	return ntohs(networkOrder);
}

// add buffer to input stream, and process the updated input stream
void TCPDatagramBuilder::feed(string buffer) {
	this->currentString += buffer;
	this->process();
}

void TCPDatagramBuilder::feed(char* buffer, int n) {
	this->currentString.append(buffer, n);
	this->process();
}

// process input stream
void TCPDatagramBuilder::process()  {

	BuilderState before = this->currentState;

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

			{
				uint16_t flags = TCPFieldToUInt(this->currentString.substr(0,2));

				this->datagram->ACK = !!(flags & 0b100);
				this->datagram->SYN = !!(flags & 0b010);
				this->datagram->FIN = !!(flags & 0b001);
				this->currentString = this->currentString.substr(2); // finished parsing the field, remove it from the input stream
			}

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
				this->complete = true;
			}
			break;

		case DONE:
			// do nothing, just let currentString grow
			this->complete = true;
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

bool TCPDatagramBuilder::isComplete() { return this->complete; }
