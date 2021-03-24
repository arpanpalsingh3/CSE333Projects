CC=gcc
CFLAGS=-std=c17 -g -Wall -c

SOURCES=Cypher.c
TARGET=Cypher.o

# the first target is the default target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

# convention says there's always a 'clean' target
clean:
	rm -f *~ $(TARGET)
