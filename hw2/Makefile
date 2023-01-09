OUTPUT=ls tree find

CFLAGS=-g -Wall -Wvla -Werror -fsanitize=address -std=gnu17
LFLAGS=-lm

%: %.o %.c
	gcc $(CFLAGS) -c -o $@ $< $(LFLAGS)

all: $(OUTPUT)

ls: ls.o getdir.o
	gcc $(CFLAGS) -o $@ ls.o getdir.o

tree: tree.o getdir.o
	gcc $(CFLAGS) -o $@ tree.o getdir.o

find: find.o getdir.o
	gcc $(CFLAGS) -o $@ find.o getdir.o


clean:
	rm -f *.o $(OUTPUT)
