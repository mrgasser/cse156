FLAGS = -Wall -Wextra -Wpedantic -Wshadow

all : myclient myserver
	mv myclient ./bin
	mv myserver ./bin

myclient: ./src/myclient.c
	clang ${FLAGS} -o myclient ./src/myclient.c

myserver: ./src/myserver.c
	clang ${FLAGS} -o myserver ./src/myserver.c

clean:
	-rm ./bin/myserver
	-rm ./bin/myclient