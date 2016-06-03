# Makefile
GPP=g++
CFLAGS= -g -Wall -Werror -std=c++0x

# Targets
all: client server
client: client.o Common.o
	$(GPP) client.o Common.o -o client.out
server: server.o Common.o serverFunctions.o
	$(GPP) server.o Common.o serverFunctions.o -o server.out
client.o:
	$(GPP) $(CFLAGS) -c client.cpp -o client.o   
server.o:
	$(GPP) $(CFLAGS) -c server.cpp -o server.o
Common.o:
	$(GPP) $(CFLAGS) -c Common.cpp -o Common.o
serverFunctions.o:
	$(GPP) $(CFLAGS) -c serverFunctions.cpp -o serverFunctions.o
TCPDatagramBuilder.o:
	$(GPP) $(CFLAGS) -c TCPDatagramBuilder.cpp -o TCPDatagramBuilder.o
parsertest: TCPDatagramBuilder.o
	$(GPP) $(CFLAGS) -c parserTest.cpp -o parsertest.o
	$(GPP) parsertest.o TCPDatagramBuilder.o -o ptest.out
ptest: Header.o
	$(GPP) $(CFLAGS) -c testHeader.cpp -o testHeader.o
	$(GPP) testHeader.o Header.o -o testHeader.out   
Header.o:
	$(GPP) $(CFLAGS) -c Header.cpp -o Header.o
clean:
	rm -f *.o *.out
