.PHONY:exe 

PROGRAM = cacheline
SHELL  	= /bin/bash
CC 		= gcc
OBJECTS = $(PROGRAM).o
CFLAGS 	= -march=native -O0
LDFLAGS = -lm -lrt

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS)
	
%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o $(PROGRAM)

