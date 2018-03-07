# Makefile for the "Maze Challenge" project.
#
# Rachel Martin, Steven Karson, Ross Guju, Rafael Brantley 3/8/18, CS 50

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -ggdb
PROG = amazingClient
OBJS = amazingClient.o startup.o localMaze.o
LIBS = -lpthread

.PHONY: all valgrind clean

all: amazingClient 

# executable depends on object files
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(PROG)

amazingClient.o: startup.h localMaze.h amazing.h
localMaze.o: localMaze.h 
startup.o: startup.h amazing.h

# check for memory leaks with valgrind	
valgrind: $(PROG)
	valgrind --leak-check=full --show-leak-kinds=all ./$(PROG) flume.cs.dartmouth.edu 17235 3 1 Amazing_rmartin_3_1.log rmartin

clean:
	rm -f $(PROG)
	rm -f *~ *.o
	rm -rf *.dSYM
