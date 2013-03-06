// EchoTcpClient.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"

void usage() {
  cout << "Usage: EchoTcpClient <name-server-port> <echo-server-domain-name-to-resolve>" << endl;
  exit(1);
}

//MODIFICATIONS 2.3.3 & 2.3.5
//int main() {
  int main(int argc, char** argv) {
      if (argc != 3) usage();
/*
	// MODIFICATION 2.3.3
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		client->connect("127.0.0.1", 4321);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
*/
	// MODIFICATION 2.3.5
	/*
	for (int i=0; i<100; i++) {
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		cout << "Petición: " << i << endl;
		client->connect("127.0.0.1", 4321);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
	}*/
	
	//modification 2.3.7
	
  // CONEXIÓN AL SERVIDOR DE NOMBRES PARA PEDIR DETALLES DEL SERVIDOR DE ECO
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
      string msg = "¡Hello CASO students!";
      client->send(msg);
      cout << "Message sent: " << msg << endl;
      msg = client->receive();
      cout << "Message received: " << msg << endl;
      client->close();	
      delete client;
  }
}

