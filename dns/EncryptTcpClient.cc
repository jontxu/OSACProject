// EncryptClient.cc

#include "TcpListener.h"
#include <fstream>

void usage() {
  cout << "Usage: EchoTcpClient <name-server-port> <echo-server-domain-name-to-resolve> <file-name>" << endl;
  exit(1);
}

int main(int argc, char** argv) {
      if (argc != 4) usage();

	
  // CONEXIÓN AL SERVIDOR DE NOMBRES PARA PEDIR DETALLES DEL SERVIDOR DE ECO
  int accion = 0;
  string key;
  char word[50];
  string text = "";
  string fileName = argv[3];
  ifstream myFile;
  ofstream newFile;
  ofstream newDFile;
  myFile.open(fileName.c_str());
  if(myFile.is_open()){
    cout << "File open successful" << endl;
    while(myFile >> word){
      text += word;
      text += " ";
    }
    myFile.close();
  }else{
    cout << "Couldn't open file" << endl;
    exit(1);    
  }
  
  PracticaCaso::TcpClient cliente;
  cliente.connect("127.0.0.1", atoi(argv[1]));
  string dnsName = argv[2];
  cliente.send(dnsName);
  string ipAddressAndPort = cliente.receive();
  cliente.close();

  if (ipAddressAndPort.find("ERROR") == 0)
      cout << "The DNS name " << dnsName << " could not be resolved." << endl;
  else {
      // CONEXIÓN AL SERVIDOR DE ECO OBTENIDO DEL SERVIDOR DE NOMBRES
      PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
      string ip = ipAddressAndPort.substr(0, ipAddressAndPort.find(":"));
      int port = atoi((ipAddressAndPort.substr(ipAddressAndPort.find(":")+1)).c_str());
      client->connect( ip, port );
      
      while(accion == 0){
	cout << "1. Encrypt with personal key" << endl;
	cout << "2. Encrypt with automatic key" << endl;
	cout << "3. Decrypt" << endl;
	cout << "Choose your option: ";
	cin >> accion;
	cout << endl;
      }
      
      switch(accion){
	case 1:
	  cout << "Encrypting key: ";
	  cin >> key;
	  cout << endl;
	  client->send("0."+key+":"+text);
	  text = client->receive();	  
	  newFile.open(("E"+fileName).c_str());
	  newFile << text;
	  newFile.close();
	  cout << "Encrypted file created with name: E"+fileName << endl;
	  break;
	case 2:
	  cout << "Your key is: 3" << endl;
	  client->send("0.3:"+text);
	  text = client->receive();
	  newFile.open(("E"+fileName).c_str());
	  newFile << text;
	  newFile.close();
	  cout << "Encrypted file created with name: E"+fileName << endl;
	  break;
	case 3:
	  cout << "Decrypting key: ";
	  cin >> key;
	  cout << endl;
	  client->send("1."+key+":"+text);
	  text = client->receive();
	  int size = strlen(text.c_str());
	  text = text.substr(0,size-1);
	  newDFile.open(("D"+fileName).c_str());
	  newDFile << text;
	  newDFile.close();
	  cout << "Decrypted file created with name: D"+fileName << endl;
	  break;
      }
	
      client->close();	
      delete client;
  }
}

