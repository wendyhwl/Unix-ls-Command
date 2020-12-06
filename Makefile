CC=gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: myls

clean:
	rm -rf myls

valgrind: all
	valgrind --leak-check=full ./s-talk

