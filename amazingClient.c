/* 
 * amazingClient - Connects to the server and starts each thread to represent Avatars and solve the maze 
 *
 * 
 * usage: ./amazingClient Hostname 17235 nAvatars Difficulty logname user

 * 
 * Rachel Martin, Raphael Brantley, Steven, Ross Guju
 * Adapted from David Kotz inclient.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       // read, write, close
#include <strings.h>        // bcopy, bzero
#include <netdb.h>        // socket-related structures
#include <time.h>
#include "amazing.h"
#include "startup.h"

/**************** file-local constants ****************/

int createSocket(char* hostname, uint32_t MazePort);
int sendMessage(int socket, AM_Message* message);
AM_Message* recieveMessage(int socket) ;
/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;        // this program's name
  char *hostname;       // server hostname
  char* user;
  int port;         // initial server port
  int numAva;
  int diff;
  AM_Message* initRecvMessage;
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

  initRecvMessage = AM_connect(hostname, port, numAva, diff);
  if(initRecvMessage != NULL) {
    if(ntohl(initRecvMessage->type) == AM_INIT_OK) {
      MazePort = ntohl(initRecvMessage->init_ok.MazePort);
      MazeWidth = ntohl(initRecvMessage->init_ok.MazeWidth);
      MazeHeight = ntohl(initRecvMessage->init_ok.MazeHeight);
    }
    free(initRecvMessage);
  }
  else {
    fprintf(stderr, "Could not initialize Maze\n");
    exit(2);
  }
  time(&currTime);
  fprintf(log, "%s, %lu, %s", user, (unsigned long)MazePort, ctime(&currTime));
  

 /*THREADS(all of this is in the thread method)
  int threadReturnStatus = 0;

  switch(ntohl(recievedMessage->type)) {
    case AM_AVATAR_TURN:
      //update grpah;
      break;
    case AM_MAZE_SOLVED:
      //append to log it was solved;
      break;
    case AM_NO_SUCH_AVATAR:
      //
      break;
    case AM_UNKNOWN_MSG_TYPE:
      fprintf(stderr, "Server recieved an unknown message of type %lu\n", (unsigned long)ntohl(recievedMessage->AM_UNKNOWN_MSG_TYPE.BadType));
      threadReturnStatus = 3;
      break;
    case AM_UNEXPECTED_MSG_TYPE:
      fprintf(stderr, "Unexpected Message\n");
      break;
    case AM_AVATAR_OUT_OF_TURN:
      fprintf(stderr, "Avatar requested a move out of order.\n");
      break;
    case AM_TOO_MANY_MOVES:
      fprintf(stderr, "Out of turns\n");
      threadReturnStatus = 4;
      break;
    case AM_SERVER_TIMEOUT:
      fprintf(stderr, "Server timed out\n");
      threadReturnStatus = 5;
      break;
    case AM_SERVER_DISK_QUOTA:
      fprintf(stderr, "Server encountered a disk quota error.\n");
      break;
    case AM_SERVER_OUT_OF_MEM:
      fprintf(stderr, "Server was out of memory.\n");
      threadReturnStatus = 6;
      break;
  }
  if(threadReturnStatus != 0) {
    break;
  }
  */

  fclose(log);
}

int createSocket(char* hostname, uint32_t MazePort, int AvatarId )
{
  struct hostent *hostp = gethostbyname(hostname);
  if (hostp == NULL) {
    fprintf(stderr, "startup: unknown host '%s'\n", hostname);
    return -1;
  }

  // Initialize fields of the server address
  struct sockaddr_in server;  // address of the server
  server.sin_family = AF_INET;
  bcopy(hostp->h_addr_list[0], &server.sin_addr, hostp->h_length);
  server.sin_port = htons(MazePort);

  // Create socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    return -1;
  }

  // And connect that socket to that server   
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    return -1;
  }
  printf("Connected!\n");


	// Send the Avatar's first message: AM_AVATAR_READY
	AM_Message message_AM_AVATAR_READY;
	message_AM_AVATAR_READY.type = AM_AVATAR_READY;
	message_AM_AVATAR_READY.AvatarId = threadArray[AvatarId];
	// TODO: create a global array of the threads named `threadArray`

  return comm_sock;

}

int sendMessage(int socket, AM_Message* message) 
{
  AM_Message* toServer = message;
 
  if(send(socket, toServer, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be sent to the server\n");
    return -1;
  } else {
    return 0;
  }
}

AM_Message* recieveMessage(int socket) 
{
  AM_Message* fromServer = malloc(sizeof(AM_Message));
  
  if(recv(socket, fromServer, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be recieved from the server\n");
    close(socket);
    return NULL;
  } else {
      if(IS_AM_ERROR(ntohl(fromServer->type))) {
       fprintf(stderr, "An %lu error was recieved from the server.\n", (unsigned long)ntohl(fromServer->type));
       return fromServer;
       close(socket);
      } else {
        return fromServer;
    }
  }
}
