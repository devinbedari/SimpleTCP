#include <cstring>
#include <iostream>
#include <stdint.h>
#include "Header.h"

using namespace std;

int main(int argc, char const *argv[])
{
	// Gen/Parse header
	Headers a, c;

	// Gen payload
	char hello[6];
	strcpy(hello, "Hello");
	hello[5] = 0;

	char *pkt = NULL;
	char *pkt_tmtd = NULL;

	// Gen random number between 0 and 65535
	uint16_t b = genRand();

	//	Values:		   0			           7
	setHeader(b, genNextNum(65535, 1), 1, ACK|SYN|FIN, a);
	genPacket(pkt, a, hello, 6);

	cerr << "================Generated Packet===================" << endl;
	cerr << "Header Info: " << a.seq_no << " " << a.ack_no << " " << a.cwnd << " " << a.flags << endl;
	cerr << "Payload to send: Hello " << endl;


	parsePacket(pkt, c, pkt_tmtd, 6);
	
	// Check headers
	cerr << "================Transmitted Packet===================" << endl;
	cerr << "Header Info: " << c.seq_no << " " << c.ack_no << " " << c.cwnd << " " << c.flags << endl;
	cerr << "Payload: " << pkt_tmtd << endl;

	delete pkt;
	delete pkt_tmtd;

	return 0;
}