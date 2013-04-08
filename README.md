NameServer
============

This is the repository of the Operative Systems' Advanced Concepts subject practical project.

##Cloning the repository
```bash
git clone git@github.com:jontxu/OSACProject.git
```

##Setting up
In order to use the dsm services you have to build the source files in the given order. For the dns services only `util` and `dns` are required, but making all the parts is acknowledged:
```bash
cd OSACProject
cd util
make
cd ../dns
make
cd ../mdns
make
cd ../dsm
make
```

If you want to clean, use `make clean` in the respective folder.

##Executing cryptography services
```bash
cd mdns
./mNameServer 4444 es
```

```bash
cd dns
./NameServer 1234 deusto.es false
```

```bash
cd dns
./EncryptTcpListener 1337
```

```bash
cd dns
 ./EncryptTcpClient 4444 encrypt.deusto.es <filename>.txt
```

##Executing time services
You'll need to open different tabs, here are in order:
```bash
cd dns
./NameServer 1234 deusto.es false
```
```bash
cd dsm
./DsmServer 5555
```
```bash
cd dsm
./DsmTimeServer 127.0.0.1 1234 dsm.deusto.es
```
```bash
cd dsm
./DsmTimeClient 127.0.0.1 1234 dsm.deusto.es
```

##Executing the hangman game
This game is intended for three people, so 6 terminals needed:
```bash
cd dns
./NameServer 1234 deusto.es false
```
```bash
cd dsm
./DsmServer 5555
```
```bash
cd dsm
./HangmanServer 127.0.0.1 1234 dsm.deusto.es <phrase>
```

```bash
cd dsm
./HangmanClient 127.0.0.1 1234 dsm.deusto.es <1-3>
```