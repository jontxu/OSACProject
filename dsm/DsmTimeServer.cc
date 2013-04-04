// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

extern "C" {
	#include <sys/time.h>
}

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: DsmTimeServer <dsm-server-port>" << endl;
	exit(1);
}

//Frees the dms memory when the program is closed (it updates time indefinitely...)
void freememory(bool run) {
	run = false;
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	driver->dsm_free("GLOBAL_TIMESTAMP");
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
	gettimeofday (&first, &tzp);
	signal(SIGINT, freememory(run));
	// Initialize an array with 100 integers and publish it in DSM under name "Array100Nums"
	do {
		try {
			first.tv_sec = second.tv_sec;
			first.tv_usec = second.tv_usec;
			driver->dsm_malloc("GLOBAL_TIMESTAMP", sizeof(first));
			try {
				driver->dsm_put("GLOBAL_TIMESTAMP", (void *)first, sizeof(first));
				gettimeofday(&second, &tzp);
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_put(\"GLOBAL_TIMESTAMP\", timestamp, " << sizeof(first) << ")): " << dsme << endl;
				driver->dsm_free("GLOBAL_TIMESTAMP");
				exit(1);
			}
		} catch (DsmException dsme) {
			// There may be several processes doing a dsm_malloc, only the first one will succeed 
			cerr << "ERROR in dsm_malloc(\"GLOBAL_TIMESTAMP\", sizeof(" << sizeof(first) << ")): " << dsme << endl;
			exit(1);
		}
	} while ((second.tv_sec - first.tv_sec) == 1 && run);
}

