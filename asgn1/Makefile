FLAGS = -Wall -Wextra -Wpedantic -Wshadow

all : myweb
	mv myweb ./bin

myweb : ./src/myweb.o
	clang ${FLAGS} -o myweb ./src/myweb.o

myweb.o : ./src/myweb.c
	clang ${FLAGS} -c ./src/myweb.c
	mv myweb.o ./src

clean:
	-rm ./src/*.o
	-rm ./bin/myweb