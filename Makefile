CC=gcc

CFLAGS=-std=c90 -O3 -Wall -c -fmessage-length=0 -ansi
LDFLAGS=

INCDIR=./inc
INCLUDE=-I./ $(INCDIR)

SRCDIR=./src
SOURCES=main.c $(SRCDIR)/common.c $(SRCDIR)/differences.c $(SRCDIR)/huffman.c $(SRCDIR)/queue.c $(SRCDIR)/tree.c

OBJDIR=./build
OBJECTS:=$(addsuffix .o,$(basename $(SOURCES)))

EXECUTABLE=audio-compressor

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

clean: ; rm *.o $(SRCDIR)/*.o $(EXECUTABLE)

