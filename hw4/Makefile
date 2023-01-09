OUTPUT=memperf

CFLAGS=-g -Wall -Wvla -Werror -fsanitize=address -std=gnu17 
LFLAGS=-lm

%: %.o %.c
	gcc $(CFLAGS) -c -o $@ $< $(LFLAGS)

all: $(OUTPUT)

# testmymalloc: testmymalloc.o mymalloc.o
# 	gcc $(CFLAGS) -o $@ testmymalloc.o mymalloc.o
memperf: memperf.o mymalloc.o
	gcc $(CFLAGS) -o $@ memperf.o mymalloc.o
clean:
	rm -f *.o $(OUTPUT)
