#pragma once

#include <string>
#include <arpa/inet.h>

using namespace std;

const int FIELD_SIZE = 2; // for this project, tcp fields are 2 bytes

struct TCPDatagram {
	uint16_t sequenceNum = 0;
	uint16_t ackNum = 0;
	uint16_t windowSize = 0;

	// flags
	bool ACK = false; // Indicates that there the value of Acknowledgment Number field is valid
	bool SYN = false; //Synchronize sequence numbers (TCP connection establishment)
	bool FIN = false; // No more data from sender (TCP connection termination)

	string data = "";

	string intToTCPField(uint16_t x) {
		uint16_t networkOrdered = htons(x);
		string str = "";
		char charAr[2] = {0,0};
		memcpy(charAr, &networkOrdered, sizeof(uint16_t));
		str.append(charAr, 2);
		return str;
	}

	string toString() {

		string str = "";
		str += intToTCPField(sequenceNum);
		str += intToTCPField(ackNum);
		str += intToTCPField(windowSize);

		uint16_t flags = 0;
		flags |= ACK << 13;
		flags |= SYN << 14;
		flags |= FIN << 15;
		str += intToTCPField(flags);

		str += data;
		return str;
	}
};
