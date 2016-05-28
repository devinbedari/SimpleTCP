#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <locale>

class TCPDatagram {

protected:
	// Protected datamembers
	char* path;
	char* host;
	char* protocolVersion;
	char* getReq;

public:
	// Default Constructor
	TCPDatagram();
	// Parameterized Constructor
	TCPDatagram(char* fpath, char* fhost);
	// Destructor
	~TCPDatagram();
	// Generate the HTTP request
	char* genReq();

	// Clear cstrings
	void clear();

  	// setters
	void setPath(char* buf);
  	void setHost(char* buf);
  	void setProtocolVersion(char* buf);

  	// getters
	char* getPath();
  	char* getHost();
  	char* getProtocolVersion();
  	// int getErrorStatus();
};
