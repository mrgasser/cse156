Max Gasser
ID: 1684340

files:

myserver.c:
contains functionality for a server that echos UDP packets back to the client that sent them.

myclient.c:
contains functionality for a client that reads from a file and reliably send it to a server

Makefile:
This file compiles both myclient.c and myserver.c creating two executables.

README.md:
The file you are reading

To compile program type make

to run program first run the server
./bin/myserver port droprate

then run x number of clients
./bin/myclient IP port mtu windsz inputfile outputfile\
