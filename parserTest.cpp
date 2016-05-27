#include <string>
#include <cstring>
#include <iostream>
#include "HttpRequest.h"
#include "HttpResponse.h"

//#include "HttpResponse.h"
using namespace std;

int main( int argc, char *argv[] )
{
	string a = "";
	char *str;
	a += "GET /index.html HTTP/1.1\r\nHost: www-net.cs.umass.edu\r\nUser-Agent: Firefox/3.6.10\r\nAccept: text/html,application/xhtml+xml\r\nAccept-Language: en-us,en;q=0.5\r\nAccept-Encoding: gzip,deflate\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7\r\nKeep-Alive: 115\r\nConnection: keep-alive\r\n\r\n";
	str = new char [a.length()];
	strcpy(str, a.c_str());
	//cout << endl << str << endl;
	HttpGetRequest o;
	o.parseReq(str);
	cout << "Path: " << o.getPath() << endl;
	cout << endl << "Host: " << o.getHost() << endl;
	cout << endl << "Protocol Version: " << o.getProtocolVersion() << endl;
	cout << endl << "Generated HttpRequest: " << endl << o.genReq();

	cout << endl << endl << endl << endl;
	string b = "";
	char *strA;
	b += "HTTP/1.1 200 OK\r\nDate: Sun, 03 Apr 2011 19:48:33 GMT\r\nServer: Apache/1.2.5\r\nLast-Modified: Tue, 22 Jun 2010 19:20:37 GMT\r\nETag: \"2b3e-258f-4c210d05\"\r\nContent-Length: 5\r\nAccept-Ranges: bytes\r\nContent-Type: text/html\r\n\r\nHello";
	strA = new char [b.length()];
	strcpy(strA, b.c_str());
	HttpResponse p;
	p.parseReq(strA);
	cout << "Protocol Version: " << p.getProtocolVersion() << endl;
	cout << endl << "Status Code: " << p.getStatusCode() << endl;
	cout << endl << "Status: " << p.getStatus() << endl;
	cout << endl << "Content-Type: " << p.getContentLength();
	cout << endl << "Payload: " << p.getPayload();
	cout << endl << "Generated HttpResponse: " << endl << p.genReq();

	delete str;
	delete strA;
	return 0;
}