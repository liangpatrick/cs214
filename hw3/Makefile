OUTPUT=shell

CFLAGS=-g -Wall -Wvla -Werror -fsanitize=address -std=gnu17
LFLAGS=-lm

%: %.o %.c
	gcc $(CFLAGS) -c -o $@ $< $(LFLAGS)

all: $(OUTPUT)

shell: shell.o
	gcc $(CFLAGS) -o $@ shell.o

clean:
	rm -f *.o $(OUTPUT)
