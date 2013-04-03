// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

extern "C" {
	#include <sys/time.h>
}

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: DsmTimeClient <dsm-server-port>" << endl;
	exit(1);
}

//Ends the program (it's ra n indefinitely)
void finalize(bool run) {
	run = false;
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	delete driver;
	exit(1);
}


int main(int argc, char** argv) {
	bool run = true;
	if (argc != 4) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;
	cout << "Getting time from server: " << driver->get_nid() << endl;

	struct timeval first, second; //or global_timestamp
	struct timezone tzp;
	char buffer[100];

	gettimeofday (&first, &tzp);
	signal(SIGINT, finalize(run));
	// Get the global timestamp variable every second
	while ((second.tv_sec - first.tv_sec) == 1 && run) {
		try {
			first.tv_sec = second.tv_sec;
			first.tv_usec = second.tv_usec;
			data = driver->dsm_get("GLOBAL_TIMESTAMP");
			timestamp = *(timeval)data.addr);
			strftime(buffer,100,"%d-%m-%Y, %H:%M:%S",localtime(&curtime.t_vsec));
			cout << "new system time set " << buffer << endl;
			gettimeofday(&second, &tzp);
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_get(\"GLOBAL_TIMESTAMP\") - Waiting for other process to initialise it: " << dsme << endl;
			driver->dsm_wait("GLOBAL_TIMESTAMP");
		}
	} 
	
	/*
	timestamp = *(timeval)data.addr);
	cout << "***Current time: " << timestamp << "***" << endl;
	delete driver;
	*/
}

