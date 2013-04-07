// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

extern "C" {
	#include <sys/time.h>
}

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: DsmTimeClient <dsm-server-port>" << endl;
	exit(1);
}

//Ends the program (it's ra n indefinitely)
void finalize(int a) {
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 4) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;
	cout << "Getting time from server: " << driver->get_nid() << endl;

	struct timeval first, second, lapsed;
	time_t curtime; //or global_timestamp
	struct timezone tzp;
	char buffer[100];
	bool run = true;

	gettimeofday(&first, &tzp);
	signal(SIGINT, finalize);
	// Get the global timestamp variable every second
	bool timeset = true;
	while(1) {
		gettimeofday(&second, &tzp);
		if (first.tv_usec > second.tv_usec) {
			second.tv_usec += 1000000;
        	second.tv_sec--;
		}

		lapsed.tv_sec = second.tv_sec - first.tv_sec;

		if (lapsed.tv_sec > 0) {
			gettimeofday (&first, &tzp);
			second.tv_usec += 1000000;
        	second.tv_sec--;
			try {
				data = driver->dsm_get("GLOBAL_TIMESTAMP");
				curtime = *((time_t*)data.addr);
				strftime(buffer,100,"%d-%m-%Y, %H:%M:%S",localtime(&curtime));
				cout << "new system time set " << buffer << endl;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"GLOBAL_TIMESTAMP\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("GLOBAL_TIMESTAMP");
			}
		}
	}
}