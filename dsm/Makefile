# Makefile
# author: dipina@eside.deusto.es
CC=g++ -fPIC
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread
INCLUDE=-I../util

main: libesidedsm.so DsmServer DsmTest MatrixEvenAdder MatrixOddAdder HangmanClient HangmanServer

libesidedsm.so: Dsm.o
	$(CC) -shared $(LDFLAGS) Dsm.o -o libesidedsm.so
	cp libesidedsm.so libesidedsm.dll
	cp libesidedsm.* ../util/
		
Dsm.o: Dsm.cc Dsm.h DsmTypes.h
	$(CC) $(INCLUDE) -c Dsm.cc
	
DsmServer: DsmServer.o
	$(CC)  DsmServer.o $(LDFLAGS) -o DsmServer

DsmTest: DsmTest.o
	$(CC) DsmTest.o -L. -lesidedsm $(LDFLAGS) -o DsmTest
	
MatrixEvenAdder: MatrixEvenAdder.o
	$(CC) MatrixEvenAdder.o -L. -lesidedsm $(LDFLAGS) -o MatrixEvenAdder
	
MatrixOddAdder: MatrixOddAdder.o
	$(CC) MatrixOddAdder.o -L. -lesidedsm $(LDFLAGS) -o MatrixOddAdder

HangmanClient: HangmanClient.o
	$(CC) HangmanClient.o -L. -lesidedsm $(LDFLAGS) -o HangmanClient
	
HangmanServer: HangmanServer.o
	$(CC) HangmanServer.o -L. -lesidedsm $(LDFLAGS) -o HangmanServer

DmsTimeServer: DmsTimeServer.o
	$(CC) DmsTimeServer.o -L. -lesidedsm $(LDFLAGS) -o DmsTimeServer
	
DmsTimeClient: DmsTimeClient.o
	$(CC) DmsTimeClient.o -L. -lesidedsm $(LDFLAGS) -o DmsTimeClient

DsmTimeServer.o: DsmTimeServer.cc DsmTimeServer.h DsmTypes.h
	$(CC) $(INCLUDE) -c DsmTimeServer.cc

DsmTimeClient.o: DsmTimeClient.cc DsmTimeClient.h DsmTypes.h
	$(CC) $(INCLUDE) -c DsmTimeClient.cc

DsmServer.o: DsmServer.cc DsmServer.h DsmTypes.h
	$(CC) $(INCLUDE) -c DsmServer.cc

DsmTest.o: DsmTest.cc
	$(CC) $(INCLUDE) -c DsmTest.cc

MatrixEvenAdder.o: MatrixEvenAdder.cc
	$(CC) $(INCLUDE) -c MatrixEvenAdder.cc
	
MatrixOddAdder.o: MatrixOddAdder.cc
	$(CC) $(INCLUDE) -c MatrixOddAdder.cc

HangmanClient.o: HangmanClient.cc
	$(CC) $(INCLUDE) -c HangmanClient.cc
	
HangmanServer.o: HangmanServer.cc
	$(CC) $(INCLUDE) -c HangmanServer.cc

clean:
	rm *.o
	rm *.so
	rm *.dll
	rm DsmServer DsmTest MatrixEvenAdder MatrixOddAdder HangmanClient HangmanServer
