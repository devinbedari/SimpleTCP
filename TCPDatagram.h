#pragma once

#include <string>

using namespace std;

const int FIELD_SIZE = 2; // for this project, tcp fields are 2 bytes

struct TCPDatagram {
	unsigned int sequenceNum;
	unsigned int ackNum;
	unsigned int windowSize;

	// flags
	bool ACK; // Indicates that there the value of Acknowledgment Number field is valid
	bool SYN; //Synchronize sequence numbers (TCP connection establishment)
	bool FIN; // No more data from sender (TCP connection termination)

	string data;

	unsigned char reverseBits(unsigned char x) {

		unsigned char bigEndian = 0;
		for (int i = 0; i < 8; i++) // move through the bits of x left-to-right
			bigEndian = (bigEndian << 1) | ((x >> i) & 1); // shift bigEndian left by one, and tack on the next bit of x to the right

		return bigEndian;
	}

	// extract a byte from a certain position in an int
	unsigned char extractByte(unsigned int x, int pos) {
		return (unsigned char) (x >> (pos*8));
	}

	string intToTCPField(int x) {

		string bigEndian = "";
		for (int i = 0; i < FIELD_SIZE; i++)
			bigEndian += (char) reverseBits(extractByte(x, i)); // reverse bits in char, then append to RIGHT side, so char order is reversed too

		return bigEndian;
	}

	string toString() {

		string str = "";
		str += intToTCPField(sequenceNum);
		str += intToTCPField(ackNum);
		str += intToTCPField(windowSize);

		int flags = 0;
		flags |= ACK << 5;
		flags |= SYN << 6;
		flags |= FIN << 7;
		str += intToTCPField(flags);

		str += data;
		return str;
	}
};
