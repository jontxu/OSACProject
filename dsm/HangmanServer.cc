// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: HangmanServer <dns-server-ip> <dns-server-port> <dsm-server-name> <phrase>" << endl;
	exit(1);
}


int main(int argc, char** argv) {
	if (argc != 5) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;

	char solution[200] = {};
	strcpy(solution, argv[4]); //string to guess
	cout << "Saving the solution '" << solution << "' in memory: " << driver->get_nid() << endl;
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


	int length = strlen(solution);
	char guess[200] = {};
	try {
		driver->dsm_malloc("Guess", sizeof(guess));
		//Unknown string
		for (int i = 0; i < length; i++)
			strcat(guess,"_");
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

	int end = 0;
	try {
		driver->dsm_malloc("End", sizeof(end));
		try {
			driver->dsm_put("End", (void *)&end, sizeof(end)); 
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"End\", end, " << sizeof(end) << ")): " << dsme << endl;
			driver->dsm_free("End");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"End\", sizeof(" << sizeof(end) << ")): " << dsme << endl;
		exit(1);
	}

	//Insert player number into dsm
	int players = 3;
	try {
		driver->dsm_malloc("Players", sizeof(players));
		try {
			driver->dsm_put("Players", (void *)&players, sizeof(players));
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"Players\", , " << sizeof(players) << ")): " << dsme << endl;
			driver->dsm_free("Players");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"Players\", sizeof(" << sizeof(players) << ")): " << dsme << endl;
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
	cout << "Starting game" << endl;
	bool found = false;

	int remaining;
	char* usersolution;
	while (!found) {
		bool guessFound = false;
		while(!guessFound) {
			try {
				data = driver->dsm_get("Guess");
				usersolution = (char*)data.addr;
				guessFound = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Guess\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Guess");
			}
		}
		bool remainingFound;
		while(!remainingFound) {
			try {
				data = driver->dsm_get("Players");
				remaining = *((int*)data.addr);
				remainingFound = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Players\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Players");
			}
		}
		if (strcmp(usersolution, solution) == 0)
			found = true;
	}

	cout << "A solution has been found" << endl;
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(20);
	delete driver;
}