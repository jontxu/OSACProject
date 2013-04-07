// EncryptServerThread.cc

#include "EncryptServerThread.h"

namespace PracticaCaso
{
	EncryptServerThread::~EncryptServerThread() {
		delete this->client;
	}

	void EncryptServerThread::run() {
		string msg = (this->client)->receive();
		string data = msg.substr(0, msg.find(":"));
		string text = msg.substr(msg.find(":")+1);
		int key = atoi((data.substr(data.find(".")+1)).c_str());
		// value = 0 Encrypt || value = 1 Decrypt
		int value = atoi((data.substr(0, data.find("."))).c_str());
		cout << "Text received correctly" << endl;
		int size = strlen(text.c_str());
		if(value == 0){
		  for(int i=0;i<size;i++){
		    text[i] = text[i] + key;    
		  }
		}else{
		  for(int i=0;i<size;i++)
		    text[i] = text[i] - key;
		}
		(this->client)->send(text);
		cout << "Text send correctly" << endl;
		(this->client)->close();
	}
}
