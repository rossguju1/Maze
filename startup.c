/* 
 * statup.c - module to connect to the server with the number of avatars and the difficulty
 *
 * Makes a connection to the given host/port and sends a AM_INIT message
 * recieves AM_INIT_OK and returns the MazePort, MazeHeight, and MazeWidth to the server
 * 
 * Rachel Martin, Raphael Brantley, Steven Karson, Ross Guju
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
AM_Message*
AM_connect(const char *hostname, const int port, const int numAva, const int diff)
{
  AM_Message initial;
  AM_Message* recievedMessage = malloc(sizeof(AM_Message));

  initial.type = htonl(AM_INIT);
  initial.init.nAvatars = htonl(numAva);
  initial.init.Difficulty = htonl(diff);

  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(hostname);
  if (hostp == NULL) {
    fprintf(stderr, "startup: unknown host '%s'\n", hostname);
    return NULL;
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
    close(comm_sock);
    return NULL;
  }

  // And connect that socket to that server   
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    close(comm_sock);
    return NULL;
  }
  printf("Connected!\n");

  // Send the AM_INIT message
  if(send(comm_sock, &initial, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Message could not be sent to the server\n");
    close(comm_sock);
    return NULL;
  }
  
  // Recieve the AM_INIT_OK message
  if(recv(comm_sock, recievedMessage, sizeof(AM_Message), 0) == -1) {
    fprintf(stderr, "Initialization message could not be recieved from the server\n");
    close(comm_sock);
    return NULL;
  } else {
      if(ntohl(recievedMessage->type) == AM_INIT_OK) {
      close(comm_sock);
       return recievedMessage;
      } else {
        fprintf(stderr, "Initialization message could not be understood from the server\n");
        close(comm_sock);
        return NULL;
    }
  }
}
