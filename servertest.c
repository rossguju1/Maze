
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       // read, write, close
#include <strings.h>        // bcopy, bzero
#include <netdb.h>        // socket-related structures
#include <time.h>
#include "../cs50W18-8/amazing.h"
#include "../cs50W18-8/startup.h"
int createSocket(char* hostname, uint32_t MazePort);
int sendMessage(int socket, AM_Message* message);
AM_Message* recieveMessage(int socket) ;

int
main(const int argc, char *argv[])
{
  int socket;
  AM_Message initial;
  AM_Message* message;
  socket = createSocket("flume.cs.dartmouth.edu", 17235);
  initial.type = htonl(AM_INIT);
  initial.init.nAvatars = htonl(4);
  initial.init.Difficulty = htonl(0);
  if(socket != -1) {
    //sendMessage(socket &initial);
    write(socket, &initial, sizeof(AM_Message));
  }
  message = recieveMessage(socket);
  printf("%lu", (unsigned long)ntohl(message->type));
  close(socket);
  free(message);
}

int createSocket(char* hostname, uint32_t MazePort)
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
  printf("Connected!\n");


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
 
  if(write(socket, toServer, sizeof(AM_Message)) == -1) {
    fprintf(stderr, "Message could not be sent to the server\n");
    return -1;
  } else {
    return 0;
  }
}

AM_Message* recieveMessage(int socket) 
{
  AM_Message* fromServer = malloc(sizeof(AM_Message));
  
  if(read(socket, fromServer, sizeof(AM_Message)) == -1) {
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