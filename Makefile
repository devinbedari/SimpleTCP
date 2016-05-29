# Makefile
GPP=g++
CFLAGS= -g -Wall -Werror

# Targets
all: client server
client: client.o Common.o
	$(GPP) client.o Common.o -o client.out
server: server.o Common.o
	$(GPP) server.o Common.o -o server.out
client.o:
	$(GPP) $(CFLAGS) -c client.cpp -o client.o   
server.o:
	$(GPP) $(CFLAGS) -c server.cpp -o server.o $(MTOPTIONS)
Common.o:
	$(GPP) $(CFLAGS) -c Common.cpp -o Common.o
TCPDatagramBuilder.o:
	$(GPP) $(CFLAGS) -c TCPDatagramBuilder.cpp -o TCPDatagramBuilder.o
parsertest: TCPDatagramBuilder.o
	$(GPP) $(CFLAGS) -c parserTest.cpp -o parsertest.o
	$(GPP) parsertest.o TCPDatagramBuilder.o -o ptest.out
clean:
	rm -f *.o *.out
