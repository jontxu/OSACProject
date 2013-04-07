# Makefile
# author: dipina@eside.deusto.es
CC=g++ -fPIC
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread -lesidesqlite
INCLUDE=-I../util

main: EncryptTcpListener EncryptTcpClient NameServer NameClient EncryptClient

EncryptTcpListener: EncryptServerThread.o EncryptTcpListener.o
	$(CC) EncryptServerThread.o EncryptTcpListener.o $(LDFLAGS) -o EncryptTcpListener

EncryptTcpClient: EncryptTcpClient.o
	$(CC) EncryptTcpClient.o $(LDFLAGS) -o EncryptTcpClient

NameServer: NameServer.o
	$(CC) NameServer.o $(LDFLAGS) -o NameServer

NameClient: NameClient.o
	$(CC) NameClient.o $(LDFLAGS) -o NameClient

EncryptServerThread.o: EncryptServerThread.cc EncryptServerThread.h
	$(CC) $(INCLUDE) -c EncryptServerThread.cc

EncryptTcpListener.o: EncryptTcpListener.cc
	$(CC) $(INCLUDE) -c EncryptTcpListener.cc

EncryptTcpClient.o: EncryptTcpClient.cc
	$(CC) $(INCLUDE) -c EncryptTcpClient.cc

NameServer.o: NameServer.cc NameServer.h
	$(CC) $(INCLUDE) -c NameServer.cc

NameClient.o: NameClient.cc 
	$(CC) $(INCLUDE) -c NameClient.cc



clean:
	rm *.o
	rm EncryptTcpListener EncryptTcpClient NameServer NameClient EncryptClient
	rm *.exe
