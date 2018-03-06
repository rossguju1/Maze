/* 
 * amazingClient - Connects to the server and starts each thread to represent Avatars and solve the maze 
 * 
 * usage: ./amazingClient Hostname 17235 nAvatars Difficulty logname user
 * 
 * Amazing Challenge
 * Group 8, Rachel Martin, Steven Karson, Ross Guju, Raphael Brantley, March 8, 2018 
 * Adapted from David Kotz inclient.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>       // read, write, close
#include <string.h>
#include <strings.h>        // bcopy, bzero
#include <netdb.h>        // socket-related structures
#include <time.h>
#include <pthread.h>
#include "amazing.h"
#include "startup.h"
#include "localMaze.h"

/**************** function declarations ****************/

int createSocket(char* hostname, uint32_t MazePort, int AvatarId );     //creates the maze's connection to the server
int sendMessage(int socket, AM_Message* message);
AM_Message* receiveMessage(int socket) ;
void* run_thread(void* idp);


/**************** global variables ****************/
MazeMap_t* mazeMap;
pthread_t* threadArray;
char *hostname;       // server hostname
uint32_t MazePort;
pthread_mutex_t mutex;
FILE* logfile;
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
  logfile = fopen(argv[5], "w");
  if(logfile == NULL) {
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
  fprintf(logfile, "%s, %lu, %s", user, (unsigned long)MazePort, ctime(&currTime));
  fclose(logfile);

  logfile = fopen(argv[5], "a");
  // Allocate memory for array
  threadArray = malloc(sizeof(pthread_t)*numAva); 
  int ids[numAva];

  for ( int i = 0; i < numAva; i++) {
    ids[i] = i;
    pthread_create(&threadArray[i], NULL, run_thread, &ids[i]);
  }

  for ( int j = 0; j < numAva; j++ ) {
    pthread_join(threadArray[j], (void**)&threadReturn);
    if(j != numAva-1) {
      free(threadReturn);
    }
  }
  free(threadArray);
  printf("Thread return status is %d\n", *threadReturn);
  if(threadReturn != NULL) {
    if(*threadReturn == 1) {
      fprintf(logfile, "We solved the maze. Yay!\n");
    } else if (*threadReturn == 2) {
      fprintf(logfile, "AM_NO_SUCH_AVATAR message received\n");
    } else if (*threadReturn == 3) {
      fprintf(logfile, "AM_UNKNOWN_MSG_TYPE message received\n");
    } else if (*threadReturn == 4) {
      fprintf(logfile, "AM_TOO_MANY_MOVES message received\n");
    } else if (*threadReturn == 5) {
      fprintf(logfile, "AM_SERVER_TIMEOUT message received\n");
    } else if (*threadReturn == 6) {
      fprintf(logfile, "AM_SERVER_OUT_OF_MEM message received\n");
    } else if (*threadReturn == 7) {
      fprintf(logfile, "There was a problem creating the socket or sending the AVATAR_READY message\n");
    }
  }
  free(threadReturn);
  fclose(logfile);
  deleteMaze(mazeMap);
  
}

/*
 * createSocket()- creates a connection for each Avatar to the server at the port specified by the AM_INIT_OK message recieved from AM_connect()
 *                 also sends the AM_AVATAR_READY message for the avatar calling it
    returns an integer which is the file descriptor of the socket for the avatar
*/
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

/*
 * sendMessage()- sends an AM_Message passed as an argument to the socket specified
    returns 0 if the message sent and -1 if the message could not be sent
*/
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

/*
 * recieveMessage()- Recieves a message from the server through the socket specified
      returns the message recieved from the server
      closes the socket when an error occurs of the maze was solved
*/
AM_Message* receiveMessage(int socket) 
{
  AM_Message* fromServer = malloc(sizeof(AM_Message));
  
  if(recv(socket, fromServer, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be received from the server\n");
    close(socket);
    return NULL;
  } else {
    if(IS_AM_ERROR(ntohl(fromServer->type))) {                          //if the message is any type of error
     fprintf(stderr, "An error was received from the server.\n");       //print to stderr that an error was recieved and close the socket
     return fromServer;
     close(socket);
    } else {
      if(ntohl(fromServer->type) == AM_MAZE_SOLVED) {                   //if the maze has been solved
        close(socket);                                                  //make sure to close the socket
      }
      return fromServer;
    }
  }
}

/*
 * run_thread()- the function run by all threads, each identified by their id passed as an argument
 *                creates an avatars personal socket with the server by calling createSocket
                  until an error is recieved or the maze is solved,
                  receives messages from the server and sends move messages on its turn
                  also logs the progress of each avatar
                  follows the right hand rule
      exits with a unique code indicating whether the maze was solved or what error occured.
*/
void* run_thread(void* idp) {

  int id = * (int*) idp;
  bool madeSocket = true;
  printf("Thread %d checking in!\n", id);
  AM_Message* receivedMessage;
  int pos= -1;   //position of the avatar in the maze numbering each cell from 0 to (width*height) going from left to right and then down each row
  int dir = 0;   //directions: north=0,east=1,south=2,west=3;
  int desiredDir = 1;
  int *threadReturnStatus = malloc(sizeof(int));
  if (threadReturnStatus == NULL) {
    pthread_exit(NULL);
  }
  //int threadReturnStatus = 0;     //indicates whether the maze was solved or an error occured
  pthread_mutex_lock(&mutex);
  int avatarSocket = createSocket(hostname, MazePort, id);    //creates the avatars socket and sends the AVATAR_READY message
  pthread_mutex_unlock(&mutex);
  if(avatarSocket == -1) {                                //if there was a problem creating the socket or sending the AVATAR_READY message
    *threadReturnStatus = 7;
    madeSocket = false;
  }
  
  //until the maze is solved or an error message occurs, recieve a message and send a message on your turn
  while (madeSocket) {
    receivedMessage = receiveMessage(avatarSocket);
    if(receivedMessage != NULL) {
      switch(ntohl(receivedMessage->type)) {
       case AM_AVATAR_TURN:
         printf("%d received turn message\n", id);
         if(ntohl(receivedMessage->avatar_turn.TurnId) == id){                    //if it is your turn
          printf("Turn of %d id. x pos is %d, y pos is %d\n", id, ntohl(receivedMessage->avatar_turn.Pos[id].x), ntohl(receivedMessage->avatar_turn.Pos[id].y));
          AM_Message turnMe;
          turnMe.type = htonl(AM_AVATAR_MOVE);
          turnMe.avatar_move.AvatarId = htonl(id);
          int serverPos = ntohl(receivedMessage->avatar_turn.Pos[id].y) * getWidth(mazeMap) + ntohl(receivedMessage->avatar_turn.Pos[id].x);

          //check where the server says you are and if you have moved from your last turn      
          if(serverPos == (getWidth(mazeMap))*(getHeight(mazeMap)/2)+(getWidth(mazeMap)/2)) {                   //Don't move if you are in the center of the maze
            turnMe.avatar_move.Direction = htonl(M_NULL_MOVE);
            fprintf(logfile, "Avatar %d is already where it needs to be so it sent a null move request\n", id);
          } else if(pos == -1) {                                                                //If this is the first message recieved, set your position
            pos = serverPos;
          } else if (pos != serverPos) {                                                        //If your move was accepted,
            fprintf(logfile, "Last move for avatar %d was accepted\n", id);                     //update the direction you are facing in your local maze
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
            pos = serverPos;                                                                    //and update your position
            desiredDir = (dir+1)%4;                                                             //and desired direction
          } else {                                                                              //If your move was rejected, put a wall in the local maze
              fprintf(logfile, "Last move for avatar %d was into a wall\n", id);
              setMapWall(mazeMap, serverPos, desiredDir);
          }
          while(getMapWall(mazeMap, serverPos, desiredDir) == 1) {
            desiredDir = (desiredDir+7)%4;                                                      //update the desired direction once counterclockwise if you know there is a wall there
          }

          //request to move in the desired direction
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
          fprintf(logfile, "Avatar %d sent move request: %lu\n", id, (unsigned long)ntohl(turnMe.avatar_move.Direction));
         }
         break;
      //if maze was solved or a fatal error occured, change the thread return status
       case AM_MAZE_SOLVED:
          printf("Maze was solved\n");
          *threadReturnStatus = 1;
         break;
       case AM_NO_SUCH_AVATAR:
         *threadReturnStatus = 2;
         break;
       case AM_UNKNOWN_MSG_TYPE:
         fprintf(stderr, "Server received an unknown message of type %lu\n", (unsigned long)ntohl(receivedMessage->unknown_msg_type.BadType));
         *threadReturnStatus = 3;
         break;
       case AM_UNEXPECTED_MSG_TYPE:
         fprintf(stderr, "Unexpected Message\n");
         break;
       case AM_AVATAR_OUT_OF_TURN:
         fprintf(stderr, "Avatar requested a move out of order.\n");
         break;
       case AM_TOO_MANY_MOVES:
         fprintf(stderr, "Out of turns\n");
         *threadReturnStatus = 4;
         break;
       case AM_SERVER_TIMEOUT:
         fprintf(stderr, "Server timed out\n");
         *threadReturnStatus = 5;
         break;
       case AM_SERVER_DISK_QUOTA:
         fprintf(stderr, "Server encountered a disk quota error.\n");
         break;
       case AM_SERVER_OUT_OF_MEM:
         fprintf(stderr, "Server was out of memory.\n");
         *threadReturnStatus = 6;
         break;
      }
    free(receivedMessage);
    }
   //if the thread return status has been changed, exit the thread function
    if(*threadReturnStatus != 0) {
     break;      // Break out of while loop
    }
  } 
  pthread_exit((void*)threadReturnStatus);
}

