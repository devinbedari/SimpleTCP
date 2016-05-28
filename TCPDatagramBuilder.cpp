#include "TCPDatagramBuilder.h"

// Default constructor
TCPDatagramBuilder::TCPDatagramBuilder() {
	this->datagram = new TCPDatagram();
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
	parseReq(initialString);
}

char* TCPDatagramBuilder::helper(char* buffer) {
	std::string temp = "";
	for(int i=0; ((buffer[i] != '\r') && (buffer[i] != ' ')); i++) {
		temp += buffer[i];
	}

	char* dataToStore = new char [temp.length()];
	std::strcpy(dataToStore, temp.c_str());
	return dataToStore;
}

//  Parse the buffer
void TCPDatagramBuilder::parseReq(char* buffer)  {
	// Start by clearing the buffer
	this->datagram->clear();

	// C++ strings are so ez
	std::string getR = buffer;

	//std::cout << std::endl << "buffer is " << buffer << std::endl ;

	for(unsigned int i = 0; i < getR.length(); i++)
	{
		// Parse GET
		if(getR[i] == 'G')
		{
			// Try and store "GET " in the c++ string
			std::string check = "";
			check += getR[i];
			check += getR[i+1];
			check += getR[i+2];
			check += getR[i+3];

			// Check if the string that was stored is "GET "
			if(check == "GET ")
			{
				int pathSize;	// Store the length of the path string

				// Accept the path size
				char* buf = this->helper(buffer+(i+4));
				this->datagram->setPath(buf);
				// std::cout << std::endl << "Path: " << this->path << std::endl;
				
				// Compute the path size, and send in this everything after the "GET <path> "
				pathSize = std::strlen(this->datagram->getPath());

				// Parse the protocol version
				buf = this->helper(buffer+(i+5+pathSize));
				this->datagram->setProtocolVersion(buf);
			}
		}

		// Parse Host
		if(getR[i] == 'H' || getR[i] == 'h')
		{
			// Try and store "Host  " in the c++ string
			std::string check = "";
			check += getR[i];
			check += getR[i+1];
			check += getR[i+2];
			check += getR[i+3];
			check += getR[i+4];
			check += getR[i+5];

			// Make string lowercase
			for(size_t ctr = 0; ctr < check.length(); ctr++)
				check[ctr] = tolower(check[ctr]);

			// Check if the string that was stored is "Host "
			if(check == "host: ") {
				char* buf = this->helper(buffer+(i+6));
				this->datagram->setHost(buf);
			}
		}
	}

}

// Accessors
TCPDatagram* TCPDatagramBuilder::getDatagram() { return this->datagram; }
