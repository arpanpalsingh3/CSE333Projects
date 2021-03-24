CC=gcc
CFLAGS=-std=c17 -g -Wall
IFLAGS=-I../Cypher

DEPOBJS=../Cypher/Cypher.o
SOURCES=main.c
TARGET=cypherer

# the first target is the default target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(IFLAGS) $(SOURCES) $(DEPOBJS) -o $(TARGET)

# convention says there's always a 'clean' target
clean:
	rm -f *~ $(TARGET)
