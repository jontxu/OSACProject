// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

extern "C" {
	#include <sys/time.h>
}

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: DsmTimeServer <dsm-server-port>" << endl;
	exit(1);
}

//Frees the dms memory when the program is closed (it updates time indefinitely...)
void freememory(int) {
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
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
	time_t curtime;
	struct timezone tzp;
	gettimeofday (&first, &tzp);
	signal(SIGINT, freememory);
	// Initialize an array with 100 integers and publish it in DSM under name "Array100Nums"
	while(1) {
		try {
			curtime = first.tv_sec;
			driver->dsm_malloc("GLOBAL_TIMESTAMP", sizeof(curtime));
			try {
				driver->dsm_put("GLOBAL_TIMESTAMP", (void *)curtime, sizeof(curtime));

			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_put(\"GLOBAL_TIMESTAMP\", curtime, " << sizeof(curtime) << ")): " << dsme << endl;
				driver->dsm_free("GLOBAL_TIMESTAMP");
				exit(1);
			}
		} catch (DsmException dsme) {
			// There may be several processes doing a dsm_malloc, only the first one will succeed 
			cerr << "ERROR in dsm_malloc(\"GLOBAL_TIMESTAMP\", sizeof(" << sizeof(curtime) << ")): " << dsme << endl;
			exit(1);
		}
		do {
			gettimeofday(&second, &tzp);
		} while (second.tv_usec - first.tv_usec != 1000000)
		first.tv_sec = second.tv_sec;
		first.tv_usec = second.tv_usec;
	}
}

