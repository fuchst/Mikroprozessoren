.PHONY:exe 

PROGRAM = cacheline
SHELL  	= /bin/bash
CC 		= gcc
OBJECTS = $(PROGRAM).o
CFLAGS 	= -lm -march=native -O0

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS)
	
%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o $(PROGRAM)

