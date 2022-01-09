FLAGS = -Wall -Wextra -Wpedantic -Wshadow

all : myweb

myweb : myweb.o
	clang ${FLAGS} -o myweb myweb.o

myweb.o : myweb.c
	clang ${FLAGS} -c myweb.c

clean:
	-rm *.o
	-rm myweb