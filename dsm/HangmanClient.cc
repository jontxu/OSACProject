// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024
#define MAX_TURNS 3

void usage() {
	cout << "Usage: HangmanClient <dns-server-ip> <dns-server-port> <dsm-server-name> <player-number (1-3)>" << endl;
	exit(1);
}

void setturn(PracticaCaso::DsmDriver * driver, int turn, int nextturn)
{
	if (turn == MAX_TURNS)
		nextturn = 1;
	else
		nextturn++;
	try {
		driver->dsm_put("Turn", (void *)&nextturn, sizeof(nextturn)); 
	} catch (DsmException dsme) {
		cerr << "ERROR: dsm_put(\"Turn\", &turn, " << sizeof(nextturn) << ")): " << dsme << endl;
		driver->dsm_free("Turn");
		exit(1);
	}
}



int main(int argc, char** argv) {

	if (argc != 5) {
		usage();
	}

	if (atoi(argv[4]) > MAX_TURNS) {
		usage();
	}

	//Variables for the game: attempts, failed characters list, introduced character or string (solution).
	int attempts = 3;
	int current = 0;
	char failedchars[3] = "";
	vector<string> failedwords;
	string input;
	int turn = atoi(argv[4]);
	int nextturn = atoi(argv[4]);

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver( argv[1], atoi(argv[2]), argv[3] );
	PracticaCaso::DsmData data;
	cout << "Starting hangman game:" << endl;

	bool won = false;
	int end = 0;
	int remaining = 3; //Max players
	//Actual game
	//Check if it's my turn
	while (!won && end != 1 && remaining > 0) {
		//Check if there are more players
		bool remainingFound;
		while(!remainingFound) {
			try {
				data = driver->dsm_get("Players");
				remaining = *((int*)data.addr);
				cout << remaining << endl;
				remainingFound = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Players\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Players");
			}
		}
		bool endGet = false;
		while (!endGet) {
			try {
				data = driver->dsm_get("End");
				endGet = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"End\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("End");
			}
		}
		end = *((int*)data.addr);
		bool myTurn = false;
		while (!myTurn) {
			try {
				data = driver->dsm_get("Turn");
				int currentturn = *((int*)data.addr);
				if (turn == currentturn) {
					myTurn = true;
				}
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Turn\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Turn");
			}
		}
		//Get guess
		char* guess;
		bool guessGet = false;
		while (!guessGet) {
			try {
				data = driver->dsm_get("Guess");
				guess = (char*)data.addr;
				guessGet = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Guess\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Guess");
			}
		}
		//Get solution
		char* solution;
		bool solutionGet = false;
		while (!solutionGet) {
			try {
				data = driver->dsm_get("Solution");
				solution = (char*)data.addr;
				solutionGet = true;
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_get(\"Solution\") - Waiting for other process to initialise it: " << dsme << endl;
				driver->dsm_wait("Solution");
			}
		}

		//Copy the guess for later comparison
		char initialguess[200];
		strcpy(initialguess, guess);

		// Check if others have solved it
		if (strcmp(guess, solution) == 0) {
			int gameend = 1;
			try {
				driver->dsm_put("End", (void *)&gameend, sizeof(gameend)); 
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_put(\"End\", end, " << sizeof(gameend) << ")): " << dsme << endl;
				driver->dsm_free("End");
				exit(1);
			}
			end = 1;
			setturn(driver, turn, nextturn);
		}

		if (attempts != 0 && end != 1) {
			cout << "It's your turn" << endl;
			//First the guessed word (between all players) appears
			cout << "Guess the word!: " << guess << endl;
			//Misses (missed character equals missed attempt, so 3 chars max)
			cout << "Missed letters (" << strlen(failedchars) << "): ";
			for (int i = 0; i < strlen(failedchars); i++) {
				cout << failedchars[i] << ", ";
			}
			cout << endl;
			cout << "Missed words (" << failedwords.size() << "): ";
			for (int i = 0; i < failedwords.size(); i++) {
				cout << failedwords[i];
				if (failedwords.size() > 1)
					cout << ", ";
			}
			cout << endl;

			//Read the input
			//A char: a character to enter in the solution.
			//A string: the solution itself 
			cout << "Introduce your character or the solution: (" << attempts << " attempts left): ";
			cin >> input;
			//If a character is introduced...
			if (input.length() == 1) {
				//Update the guess if the solution contains the input
				for (int i = 0; i < strlen(guess); i++) {
					if (solution[i] == (char)input.c_str()[0])
						guess[i] = (char)input.c_str()[0];
				}
				if (strcmp(initialguess, guess) == 0) {
					//If there's no update (miss) then player loses attempt
					strcat(failedchars, input.c_str());
					cout << "Incorrect character: " << input.c_str() << endl;
					attempts--;
					current++;
				} else {
					//In the other case, update in shared memory
					cout << "Correct character: " << input.c_str() << endl;
					cout << "Updated guess: " << guess << endl;
					try {
						driver->dsm_put("Guess", (void *)guess, sizeof(initialguess)); 
					} catch (DsmException dsme) {
						cerr << "ERROR: dsm_put(\"Guess\", guess, " << sizeof(initialguess) << ")): " << dsme << endl;
						driver->dsm_free("Guess");
						exit(1);
					}
					//If the updated guess equals solution, you won
					if (strcmp(guess, solution) == 0) {
						cout << "Congratulations, you beat the game in " << current << " steps." << endl;
						int gameend = 1;
						try {
							driver->dsm_put("End", (void *)&gameend, sizeof(gameend)); 
						} catch (DsmException dsme) {
							cerr << "ERROR: dsm_put(\"End\", end, " << sizeof(gameend) << ")): " << dsme << endl;
							driver->dsm_free("End");
							exit(1);
						}
						setturn(driver, turn, nextturn);
						end = 1;
						won = true;
					}
				}
			} else {
				//If a word is entered, compare it with the solution
				if (strcmp(input.c_str(),solution) == 0) {
					//If it's equal, we beat the game
					cout << "Correct word: " << input.c_str() << endl;
					cout << "Congratulations, you beat the game in " << current << " steps." << endl;
					try {
						driver->dsm_put("Guess", (void *)input.c_str(), sizeof(initialguess)); 
					} catch (DsmException dsme) {
						cerr << "ERROR: dsm_put(\"Guess\", guess, " << sizeof(initialguess) << ")): " << dsme << endl;
						driver->dsm_free("Guess");
						exit(1);
					}
					int gameend = 1;
					try {
						driver->dsm_put("End", (void *)&gameend, sizeof(gameend)); 
					} catch (DsmException dsme) {
						cerr << "ERROR: dsm_put(\"End\", end, " << sizeof(gameend) << ")): " << dsme << endl;
						driver->dsm_free("End");
						exit(1);
					}
					won = true;
					end = 1;
				}
				else {
					//If not, lost attempt
					cout << "Incorrect word: " << input.c_str() << endl;
					failedwords.push_back(input);
					current++;
					attempts--;
				}
			}
			setturn(driver, turn, nextturn);
		}

		//When lost, delete player from dsm
		if (attempts == 0 && end != 1 && remaining > 0 ) {
			bool remainingsFound;
			//Check number of players
			while(!remainingsFound) {
				try {
					data = driver->dsm_get("Players");
					remaining = *((int*)data.addr);
					cout << remaining << endl;
					remainingsFound = true;
				} catch (DsmException dsme) {
					cerr << "ERROR: dsm_get(\"Players\") - Waiting for other process to initialise it: " << dsme << endl;
					driver->dsm_wait("Players");
				}
			}
			//If it's positive, decrease (we've lost) by 1
			int remain = remaining;
			if (remaining > 0) {
				try {
					remaining--;
					remain = remaining;
					driver->dsm_put("Players", (void *)&remain, sizeof(remain)); 
				} catch (DsmException dsme) {
					cerr << "ERROR: dsm_put(\"Players\", remains, " << sizeof(remain) << ")): " << dsme << endl;
					driver->dsm_free("Players");
					exit(1);
				}
			}
			//If last player, exit
			if (remain == 1) {
				break;
			} else {
				//If not, pass turn
				setturn(driver, turn, nextturn);
				break;
			}
		}
	}

	//Game is lost
	if (attempts == 0) {
		cout << "Sorry, you have lost the game :(" << endl;
	}

	cout << "Game finished" << endl;	
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	delete driver;
}	