/* 
 * amazingClient - 
 *
 * Makes a connection to the given host/port and sends a AM_INIT message
 * recieves AM_INIT_OK and returns the MazePort, MazeHeight, and MazeWidth to the server
 * 
 * usage: inclient hostname port
 * 
 * Rachel Martin, Raphael Brantley, Steven, Ross Guju
 * Adapted from David Kotz inclient.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures
#include "amazing.h"

/**************** file-local constants ****************/

/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;	      // this program's name
  char *hostname;	      // server hostname
  int port;		      // server port
  int numAva;
  int diff;
  AM_Message recievedMessage;
  FILE* log;
  // check arguments
  program = argv[0];
  if (argc != 6) {
    fprintf(stderr, "usage: %s hostname port numAva diff\n", program);
    exit(1);
  } else {
    hostname = argv[1];
    port = atoi(argv[2]);
    numAva = atoi(argv[3]);
    diff = atoi(argv[4]);
  }
  log = fopen(argv[6], "a");
  if(log == NULL) {
  	fprintf(stderr, "Could not open log\n");
  	exit(1);
  }
  recievedMessage = AM_connect(hostname, port, numAva, diff);