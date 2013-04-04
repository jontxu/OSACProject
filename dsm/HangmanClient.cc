// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: HangmanClient <dsm-server-port> <player-number>" << endl;
	exit(1);
}


int main(int argc, char** argv) {

	if (argc != 4) {
		usage();
	}
	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;
	cout << "Starting hangman game in " << driver->get_nid() << endl;

	//Variables for the game: attempts, failed characters list, introduced character or string (solution).
	int attempts = 10;
	int current = 0;
	char failedchars[] = {};
	string input;
	int turn;
	char* solution;

	//Get solution
	bool solutionGet = false;
	while (!solutionGet) {
		try {
			data = driver->dsm_get("Solution");
			solutionGet = true;
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_get(\"Solution\") - Waiting for other process to initialise it: " << dsme << endl;
			driver->dsm_wait("Solution");
		}
	}
	solution = *((char**)data.addr);
	
	//Actual game
	while (attempts != 0) {

		//Check if it's my turn
		bool myTurn = false;
		while (!myTurn) {
			try {
				data = driver->dsm_get("Solution");
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Solution\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Solution");
			}
			int currentturn = *((int*)data.addr);
			if (turn == currentturn)
				myTurn = true;
		}

		//Get guess
		bool guessGet = false;
		while (!guessGet) {
			try {
				data = driver->dsm_get("Guess");
				guessGet = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Guess\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Guess");
			}
		}
		char* guess = *((char**)data.addr);
		// Check if others have solved it
		if (strcmp(guess, solution) == 0) {
			cout << "Game has ended" << endl;
			break;
		}
		//Print the guess and the misses (missed character equals missed attempt, so 10 chars max)
		cout << "Guess the phrase: " << guess << endl;
		cout << "Misses:" << failedchars << endl;
		//Read the input
		//A char: a character to enter in the solution.
		//A string: the solution itself
		cout << "Introduce your character or the solution. You have: " << attempts << " attempts left: " << endl;
		cin >> input;
		//We get the first guess to compare the changes. If the char doesn't exists, it's a failed attempt.
		char* initialguess = guess;
		if (input.length() == 1) {
			for (int i = 0; i < strlen(solution); i++) {
				if (solution[i] == (char)input.c_str()[0])
					guess[i] = (char)input.c_str()[0];
			}
			if (strcmp(initialguess, guess) != 0) {
				failedchars[strlen(failedchars)] = (char)input.c_str()[0];
				attempts--;
				current++;
			} else {
				try {
					driver->dsm_put("Guess", (void *)guess, sizeof(guess)); 
				} catch (DsmException dsme) {
					cerr << "ERROR: dsm_put(\"Guess\", guess, " << sizeof(guess) << ")): " << dsme << endl;
					driver->dsm_free("Guess");
					exit(1);
				}
			}
		} else {
			if (strcmp(input.c_str(), solution) == 0) {
				cout << "Congratulations, you beat the game in " << current << " attempts." << endl;
				break;
			}
			else {
				current++;
				attempts--;
			}
		}

		try {
			driver->dsm_malloc("Turn", sizeof(turn));
			//Save player number in shared memory for turn management
			turn = argv[3][0] + 1;
			if (turn == 3)
				turn = 1;
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
	}

	if (attempts == 0) {
		cout << "Sorry, you have lost the game :(";
	}
	
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	driver->dsm_free("Guess");
	driver->dsm_free("Solution");
	delete driver;
}