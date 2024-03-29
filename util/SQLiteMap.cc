// SQLiteMap.cc
// author: dipina@eside.deusto.es
// compile: g++ SQLiteMap.cc -lsqlite3 -o SQLiteMap
#include "SQLiteMap.h"

namespace PracticaCaso {
	SQLiteMap::SQLiteMap(string fn): fileName(fn), dbh(0) {
		// Process the contents of the mapping file
		this->loadMappings(fn);
	}

	SQLiteMap::SQLiteMap(const SQLiteMap& rhs) {
		fileName = rhs.fileName;
		dbh = rhs.dbh;
		this->loadMappings(fileName);
	}

	SQLiteMap::~SQLiteMap() {
		cout << "SQLiteMap: destructor called " << endl;
		this->close();
	}

	void SQLiteMap::loadMappings(string mappingsDBFileName) {
		sqlite3_stmt *pStntm;
		string statement;
		int state;
		
		// If database can't be opened, exit.
		if (sqlite3_open(mappingsDBFileName.c_str(), &dbh) != SQLITE_OK) {
			cerr << "Can't open database" << endl;
			sqlite3_close(dbh);
			exit(1);  
		}
	  
		//Try to take the rows from the table
		statement = "Select * from KeyValuePair";
		sqlite3_prepare(dbh, statement.c_str(), statement.size(), &pStntm, NULL);
		state = sqlite3_step(pStntm);
		if (state == SQLITE_ERROR) {
			//If the table doesn't exists, create it.
			statement = "create table KeyValuePair(key_element BLOB NOT NULL PRIMARY KEY,value_element BLOB)";
			sqlite3_prepare(dbh, statement.c_str(), statement.size(), &pStntm, NULL);
			if (sqlite3_step(pStntm) != SQLITE_OK) {
				//If there's any error while creating the table, exit.
				cerr << "Can't create KeyValuePair table" << endl;
				sqlite3_close(dbh);
				exit(1);
			}
		} else {
			//If there are entries on the table, get the rows:
			//We get one row at a time
			//This ends when no more rows are available.
			while (state != SQLITE_DONE && state == SQLITE_ROW) {
				//Get the row entries: key-value
				const char* key = (const char*) sqlite3_column_text(pStntm, 0);
				const char* value = (const char*) sqlite3_column_text(pStntm, 1);
				//Insert the entries into the map.
				dns2IpPortMap[key] = value;
				//Get the next row.
				state = sqlite3_step(pStntm);
			}
		}
		//End the statement
		sqlite3_finalize(pStntm);
	}

	map<string, string> SQLiteMap::getMap() {
		return dns2IpPortMap;
	}


	string SQLiteMap::get(string key) {
	  return dns2IpPortMap[key];
	}

	void SQLiteMap::set(string mapKey, string mapValue) {
	  //char ** result, * errorMsg;
		sqlite3_stmt *stmt;
		string sql;
		if (dns2IpPortMap.find(mapKey) != dns2IpPortMap.end()) {
			dns2IpPortMap.erase(mapKey);
			dns2IpPortMap[mapKey] = mapValue;
			sql = "Update KeyValuePair set value_element " + mapValue + " where key_element = " + mapKey;
		} else {
			dns2IpPortMap[mapKey] = mapValue;
			sql = "Insert into KeyValuePair values ('" + mapKey + "','" + mapValue + "')";
		}
		sqlite3_prepare(dbh, sql.c_str(), sql.size(), &stmt, NULL);
		if (sqlite3_step(stmt) != SQLITE_DONE) {
			cerr << "Can't insert or update database";
			sqlite3_close(dbh);
			exit(1);
		}
		sqlite3_finalize(stmt);
		// Undertake the update of the STL map and the database. Bear in mind that it there is not an entry with a passed key an INSERT will have to be executed, if there was already such an entry an UPDATE will take place
	}


	void SQLiteMap::close() {
		 int end = sqlite3_close(dbh);
	}

	ostream & operator << (ostream & os, SQLiteMap &t) {
		os << "DB file name: " << t.fileName << endl;
		os << "DNS mappings:" << endl;
		typedef map<string, string>::const_iterator CI;
		for (CI p = t.dns2IpPortMap.begin(); p != t.dns2IpPortMap.end(); ++p) {
			os <<  p->first << " : " << p->second << endl;
		}
		return os;
	}
}


/*
// global variable
PracticaCaso::SQLiteMap * SQLiteMap_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	delete SQLiteMap_pointer;
}


void usage() {
	cout << "Usage: SQLiteMap <name-mappings-db-file>" << endl;
	exit(1);
}

int main(int argc, char** argv) {
	signal(SIGINT,ctrl_c);

	if (argc != 2) {
		usage();
	}

	PracticaCaso::SQLiteMap * SQLiteMap = new PracticaCaso::SQLiteMap((string)argv[1]);
	cout << "SQLiteMap instance: " << endl << SQLiteMap << endl;
	//SQLiteMap_pointer = &SQLiteMap;
	SQLiteMap_pointer = SQLiteMap;

	//SQLiteMap.set("saludo", "hola");
	//SQLiteMap.set("despedida", "adios");
	SQLiteMap->set("saludo", "hola");
	SQLiteMap->set("despedida", "adios");

	//cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap.get("saludo") << endl;
	//cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap.get("despedida") << endl;
	cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap->get("saludo") << endl;
	cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap->get("despedida") << endl;

	cout << "SQLiteMap instance after 2 sets: " << endl << *SQLiteMap << endl;

	//map<string, string> savedSQLiteMap = SQLiteMap.getMap();
	map<string, string> savedSQLiteMap = SQLiteMap->getMap();
	cout << "Recuperado el mapa" << endl;
	typedef map<string, string>::const_iterator CI;
	for (CI p = savedSQLiteMap.begin(); p != savedSQLiteMap.end(); ++p) {
		cout << "dns2IpPortMap[" << p->first << "] = " << p->second << endl;
	}
	delete SQLiteMap_pointer;
}
*/
