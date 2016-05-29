#pragma once

#include <string>

using namespace std;

struct TCPDatagram {
	unsigned int sequenceNum;
	unsigned int ackNum;
	unsigned int windowSize;

	// flags
	bool ACK; // Indicates that there the value of Acknowledgment Number field is valid
	bool SYN; //Synchronize sequence numbers (TCP connection establishment)
	bool FIN; // No more data from sender (TCP connection termination)

	string data;

	// for this project, tcp fields are 2 bytes
	string intToTCPField(int x) {

		string str = to_string(x);

		if (str.length() > 2) {
			cerr << "TCP Datagram header contains " << x << " which is larger than 2 bytes" << endl;
			exit(1);
		}

		string bigEndian = "";
		// first reverse the chars, because TCP fields are given in network-byte order, aka Big Endian
		for (int i = str.length()-1; i >= 0; i--)
			bigEndian += str.at(i);

		bigEndian = (bigEndian+"00").substr(0,2); // pad right side with zeros
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
