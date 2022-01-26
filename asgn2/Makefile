FLAGS = -Wall -Wextra -Wpedantic -Wshadow

all : myclient myserver

myclient: myclient.c
	clang ${FLAGS} -o myclient myclient.c

myserver: myserver.c
	clang ${FLAGS} -o myserver myserver.c

clean:
	-rm myserver
	-rm myclient