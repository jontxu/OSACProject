// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: HangmanServer <dsm-server-port> <phrase>" << endl;
	exit(1);
}


int main(int argc, char** argv) {
	if (argc != 4) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;
	const char* solution = argv[2]; //string to guess
	cout << "Saving " << solution << "in memory: " << driver->get_nid() << endl;

	try {
		driver->dsm_malloc("Solution", sizeof(solution));
		try {
			driver->dsm_put("Solution", (void *)solution, sizeof(solution)); 
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"Solution\", solution, " << sizeof(solution) << ")): " << dsme << endl;
			driver->dsm_free("Solution");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"Solution\", sizeof(" << sizeof(solution) << ")): " << dsme << endl;
		exit(1);
	}

	//Inserts the guess string
	char* guess[strlen(solution)];
	try {
		driver->dsm_malloc("Guess", sizeof(guess));
		//Unknown string
		for (int i = 0; i < strlen(solution); i++)
			guess[i] = "_";
		try {
			driver->dsm_put("Guess", (void *)guess, sizeof(guess)); 
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"Guess\", guess, " << sizeof(guess) << ")): " << dsme << endl;
			driver->dsm_free("Guess");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"Guess\", sizeof(" << sizeof(guess) << ")): " << dsme << endl;
		exit(1);
	}

	//Player 1 starts
	int turn = 1;
	try {
		driver->dsm_malloc("Turn", sizeof(turn));
		try {
			driver->dsm_put("Turn", (void *)&turn, sizeof(turn)); 
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"Turn\", &turn, " << sizeof(turn) << ")): " << dsme << endl;
			driver->dsm_free("Turn");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"Turn\", sizeof(" << sizeof(turn) << ")): " << dsme << endl;
		exit(1);
	}

	//Game: Server finishes if the solution has been found
	bool found = false;
	while (!found) {
		try {
			data = driver->dsm_get("Guess");
			char* usersolution = *((char **)data.addr);
			if (strcmp(usersolution, solution) == 0)
				found = true;
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_get(\"Guess\") - Waiting for other process to initialise it: " << dsme << endl;
			driver->dsm_wait("Guess");
		}
	}

	cout << "A solution has been found" << endl;
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	driver->dsm_free("Guess");
	driver->dsm_free("Solution");
	delete driver;
}