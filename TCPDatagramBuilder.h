#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <locale>
#include "TCPDatagram.h"

class TCPDatagramBuilder {

protected:
	// Protected datamembers
	TCPDatagram* datagram;

	// Helper fuction
	char* helper(char* buffer);

public:
	// Default Constructor
	TCPDatagramBuilder();
	// Constructor to initialize a base datagram
	TCPDatagramBuilder(TCPDatagram* datagram);
	// Constructor with an initial string to parse
	TCPDatagramBuilder(char* initialString);
	// Destructor
	~TCPDatagramBuilder();

	// Parse request
	void parseReq(char *buffer);

  	// Return protected variables
	TCPDatagram* getDatagram();
};
