/* 
 * statup - connects to the server with the number of avatars and the difficulty
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
  AM_Message initial;
  AM_Message recievedMessage;
  // check arguments
  program = argv[0];
  if (argc != 5) {
    fprintf(stderr, "usage: %s hostname port numAva diff\n", program);
    exit(1);
  } else {
    hostname = argv[1];
    port = atoi(argv[2]);
    numAva = atoi(argv[3]);
    diff = atoi(argv[4]);
  }

  initial.type = htonl(AM_INIT);
  initial.init.nAvatars = htonl(numAva);
  initial.init.Difficulty = htonl(diff);
  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(hostname);
  if (hostp == NULL) {
    fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
    exit(3);
  }

  // Initialize fields of the server address
  struct sockaddr_in server;  // address of the server
  server.sin_family = AF_INET;
  bcopy(hostp->h_addr_list[0], &server.sin_addr, hostp->h_length);
  server.sin_port = htons(port);

  // Create socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    exit(2);
  }

  // And connect that socket to that server   
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    exit(4);
  }
  printf("Connected!\n");

  if(send(comm_sock, &initial, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be sent to the server\n");
    exit(5);
  }
  
  if(recv(comm_sock, &recievedMessage, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Initialization message could not be recieved from the server\n");
    exit(6);
  } else {
    if(ntohl(recievedMessage.type) == AM_INIT_OK) {
      return ntohl(recievedMessage.init_ok.MazePort);
    } else {
       fprintf(stderr, "Initialization message could not be understood from the server\n");
        exit(6);
    }
  }
}