#pragma once

#include <string>

struct TCPDatagram {
	unsigned int sequenceNum;
	unsigned int ackNum;
	unsigned int windowSize;

	// flags
	bool ACK; // Indicates that there the value of Acknowledgment Number field is valid
	bool SYN; //Synchronize sequence numbers (TCP connection establishment)
	bool FIN; // No more data from sender (TCP connection termination)

	std::string data;
};
