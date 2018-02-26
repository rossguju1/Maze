/* 
 * amazingClient - 
 *
 * 
 * usage: ./amazingClient Hostname 17235 nAvatars Difficulty logname user

 * 
 * Rachel Martin, Raphael Brantley, Steven, Ross Guju
 * Adapted from David Kotz inclient.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures
#include <time.h>
#include "amazing.h"
#include "startup.h"

/**************** file-local constants ****************/

/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;	      // this program's name
  char *hostname;	      // server hostname
  char* user;
  int port;		      // server port
  int numAva;
  int diff;
  AM_Message* recievedMessage;
  FILE* log;
  uint32_t MazePort;
  uint32_t MazeWidth;
  uint32_t MazeHeight;
  time_t currTime;
    
  // check arguments
  program = argv[0];
  if (argc != 7) {
    fprintf(stderr, "usage: %s hostname port numAva diff\n", program);
    exit(1);
  } else {
    hostname = argv[1];
    port = atoi(argv[2]);
    numAva = atoi(argv[3]);
    diff = atoi(argv[4]);
    user = argv[6];
  }
  log = fopen(argv[5], "w");
  if(log == NULL) {
  	fprintf(stderr, "Could not open log\n");
  	exit(1);
  }

  recievedMessage = AM_connect(hostname, port, numAva, diff);
  if(recievedMessage != NULL) {
    if(ntohl(recievedMessage->type) == AM_INIT_OK) {
      MazePort = ntohl(recievedMessage->init_ok.MazePort);
      MazeWidth = ntohl(recievedMessage->init_ok.MazeWidth);
      MazeHeight = ntohl(recievedMessage->init_ok.MazeHeight);
    }
    free(recievedMessage);
  }
  else {
    fprintf(stderr, "Could not initialize Maze\n");
    exit(2);
  }
  time(&currTime);
  fprintf(log, "%s, %lu, %s", user, (unsigned long)MazePort, ctime(&currTime));
  

 //THREADS


  fclose(log);
  

}

