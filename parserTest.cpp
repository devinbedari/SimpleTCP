#include <string>
#include <cstring>
#include <iostream>
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

int main( int argc, char *argv[] ) {

	string a = "";
	char *str;
	a += "GET /index.html HTTP/1.1\r\nHost: www-net.cs.umass.edu\r\nUser-Agent: Firefox/3.6.10\r\nAccept: text/html,application/xhtml+xml\r\nAccept-Language: en-us,en;q=0.5\r\nAccept-Encoding: gzip,deflate\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7\r\nKeep-Alive: 115\r\nConnection: keep-alive\r\n\r\n";
	str = new char [a.length()];
	strcpy(str, a.c_str());
	//cout << endl << str << endl;
	TCPDatagramBuilder b;
	b.parseReq(str);
	TCPDatagram o = *(b.getDatagram());
	cout << "Path: " << o.getPath() << endl;
	cout << endl << "Host: " << o.getHost() << endl;
	cout << endl << "Protocol Version: " << o.getProtocolVersion() << endl;
	cout << endl << "Generated HttpRequest: " << endl << o.genReq() << endl;

	delete str;
	return 0;
}
