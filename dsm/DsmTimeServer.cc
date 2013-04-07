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

	struct timeval first, second, lapsed, current; //or global_timestamp
	time_t curtime;
	struct timezone tzp;
	char buffer[100];
	bool timeset = true;
	gettimeofday (&first, &tzp);
	signal(SIGINT, freememory);
	try {
		curtime = first.tv_sec;
		cout << "I arrived to 2" << endl;
		driver->dsm_malloc("GLOBAL_TIMESTAMP", sizeof(curtime));
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"GLOBAL_TIMESTAMP\", sizeof(" << sizeof(curtime) << ")): " << dsme << endl;
		exit(1);
	}
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
				gettimeofday(&current, &tzp);
				curtime = current.tv_sec;
				strftime(buffer,100,"%d-%m-%Y, %H:%M:%S",localtime(&curtime));
				driver->dsm_put("GLOBAL_TIMESTAMP", (void *)&curtime, sizeof(curtime));
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_put(\"GLOBAL_TIMESTAMP\", curtime, " << sizeof(curtime) << ")): " << dsme << endl;
				driver->dsm_free("GLOBAL_TIMESTAMP");
				exit(1);
			}
		}
	}
}
