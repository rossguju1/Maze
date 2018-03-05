/* /threa
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
#include <string.h>
#include <strings.h>        // bcopy, bzero
#include <netdb.h>        // socket-related structures
#include <time.h>
#include <pthread.h>
#include "amazing.h"
#include "startup.h"
#include "localMaze.h"

/**************** file-local constants ****************/

int createSocket(char* hostname, uint32_t MazePort, int AvatarId );
int sendMessage(int socket, AM_Message* message);
AM_Message* receiveMessage(int socket) ;
void* run_thread(void* idp);


/**************** global variables ****************/
MazeMap_t* mazeMap;
pthread_t* threadArray;
char *hostname;       // server hostname
uint32_t MazePort;
pthread_mutex_t mutex;
/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;        // this program's name
  
  char* user;
  int port;         // initial server port
  int numAva;
  int diff;
  AM_Message* initRecvMessage;
  FILE* log;
  
  uint32_t MazeWidth;
  uint32_t MazeHeight;
  time_t currTime;
  
  int* threadReturn;
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

  mazeMap = initMazeMap((int)MazeWidth, (int) MazeHeight);

  
  time(&currTime);
  fprintf(log, "%s, %lu, %s", user, (unsigned long)MazePort, ctime(&currTime));
  fclose(log);

  // Allocate memory for array
  threadArray = malloc(sizeof(pthread_t)*numAva); //TODO: free memory
  int ids[numAva];

  for ( int i = 0; i < numAva; i++) {
    ids[i] = i;
    pthread_create(&threadArray[i], NULL, run_thread, &ids[i]);
  }

  for ( int j = 0; j < numAva; j++ ) {
    pthread_join(threadArray[j], (void**)&threadReturn);
  }

  printf("Thread return status is %d\n", *threadReturn);
  if(*threadReturn == 1) {
    printf("YUSSS We solved it\n");
  }

  deleteMaze(mazeMap);
  fclose(log);
}

int createSocket(char* hostname, uint32_t MazePort, int AvatarId )
{
  AM_Message readyMessage;
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
  printf("Avatar %d connected to our maze's port! Its socket number is %d\n", AvatarId, comm_sock);


	// Send the Avatar's first message: AM_AVATAR_READY
	
	readyMessage.type = htonl(AM_AVATAR_READY);
	readyMessage.avatar_ready.AvatarId = htonl(AvatarId);
	
  if(sendMessage(comm_sock, &readyMessage) == -1){
    return -2;
  }

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

AM_Message* receiveMessage(int socket) 
{
  AM_Message* fromServer = malloc(sizeof(AM_Message));
  
  if(recv(socket, fromServer, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be received from the server\n");
    close(socket);
    return NULL;
  } else {
      if(IS_AM_ERROR(ntohl(fromServer->type))) {
       fprintf(stderr, "An error was received from the server.\n");
       return fromServer;
       close(socket);
      } else {
        return fromServer;
    }
  }
}

void* run_thread(void* idp) {

  int id = * (int*) idp;
  printf("Thread %d checking in!\n", id);
  AM_Message* receivedMessage;
  int pos= -1;
  int dir = 0;   //directions: north=0,east=1,south=2,west=3;
  int desiredDir = 1;
  pthread_mutex_lock(&mutex);
  int avatarSocket = createSocket(hostname, MazePort, id);
  pthread_mutex_unlock(&mutex);
  int threadReturnStatus = 0;
  

  //TODO: Add while loop that runs as long as maze is unsolved and there are moves left
  while (1) {
    receivedMessage = receiveMessage(avatarSocket);
    if(receivedMessage != NULL) {
      switch(ntohl(receivedMessage->type)) {
       case AM_AVATAR_TURN:
         printf("%d received turn message\n", id);
         if(ntohl(receivedMessage->avatar_turn.TurnId) == id){
          printf("Turn of %d id. x pos is %d, y pos is %d\n", id, ntohl(receivedMessage->avatar_turn.Pos[id].x), ntohl(receivedMessage->avatar_turn.Pos[id].y));
          AM_Message turnMe;
          turnMe.type = htonl(AM_AVATAR_MOVE);
          turnMe.avatar_move.AvatarId = htonl(id);
          int serverPos = ntohl(receivedMessage->avatar_turn.Pos[id].y) * getWidth(mazeMap) + ntohl(receivedMessage->avatar_turn.Pos[id].x);

          if(serverPos == (getWidth(mazeMap))*(getHeight(mazeMap)/2)+(getWidth(mazeMap)/2)) {
            turnMe.avatar_move.Direction = htonl(M_NULL_MOVE);
          } else if(pos == -1) {
            pos = serverPos;
          } else if (pos != serverPos) {
            if(serverPos == pos-getWidth(mazeMap)) {
              dir = 0;
            }
            if(serverPos == pos+1) {
              dir = 1;
            }
            if(serverPos == pos+getWidth(mazeMap)) {
              dir = 2;
            }
            if(serverPos == pos-1) {
              dir = 3;
            }
            pos = serverPos;
            desiredDir = (dir+1)%4;
          } else {
              setMapWall(mazeMap, serverPos, desiredDir);
          }
          while(getMapWall(mazeMap, serverPos, desiredDir) == 1) {
            desiredDir = (desiredDir+7)%4;
          }
          if(desiredDir == 0) {
            turnMe.avatar_move.Direction = htonl(M_NORTH);
          } else if(desiredDir == 1) {
            turnMe.avatar_move.Direction = htonl(M_EAST);
          } else if(desiredDir == 2) {
            turnMe.avatar_move.Direction = htonl(M_SOUTH);
          } else if(desiredDir == 3) {
            turnMe.avatar_move.Direction = htonl(M_WEST);
          }
          
          sendMessage(avatarSocket, &turnMe);
         }
         break;
       case AM_MAZE_SOLVED:
        //append to log it was solved;
          printf("Maze was solved\n");
          threadReturnStatus = 1;
         break;
       case AM_NO_SUCH_AVATAR:
         threadReturnStatus = 2;
         break;
       case AM_UNKNOWN_MSG_TYPE:
         fprintf(stderr, "Server received an unknown message of type %lu\n", (unsigned long)ntohl(receivedMessage->unknown_msg_type.BadType));
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
     free(receivedMessage);

     
   }
   if(threadReturnStatus != 0) {
     // Break out of while loop
      break;
     }

  } 

  pthread_exit(&threadReturnStatus);
}

