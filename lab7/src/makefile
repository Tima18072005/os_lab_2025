CC = gcc

all: udpserver udpclient tcpserver tcpclient

udpserver: udpserver.c
	$(CC) -o udpserver udpserver.c

tcpserver: tcpserver.c
	$(CC) -o tcpserver tcpserver.c

udpclient: udpclient.c 
	$(CC) -o udpclient udpclient.c

tcpclient: tcpclient.c 
	$(CC) -o tcpclient tcpclient.c 

clean:
	rm -f udpserver udpclient tcpserver tcpclient
