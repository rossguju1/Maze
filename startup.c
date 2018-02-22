/* 
 * statup - connects to the server with the number of avatars and the difficulty
 *
 * Makes a connection to the given host/port and sends a AM_INIT message
 * recieves AM_INIT_OK and returns the MazePort, MazeHeight, and MazeWidth to the server
 * 
 * usage: inclient hostname port
 * 
 * David Kotz, 1987, 1992, 2016
 * Adapted from Figure 7a in Introductory 4.3bsd IPC, PS1:7-15.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures

/**************** file-local constants ****************/
static const int BUFSIZE = 1024;     // read/write buffer size

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
  initial.init.difficulty = htonl(diff);
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

  // read content from stdin (file descriptor = 0) and write to socket
  char buf[BUFSIZE];	      // a buffer for reading data from stdin
  int bytes_read;	      // #bytes read from socket
  do {
    if(write(comm_sock, ) < 0)
    
    if ((bytes_read = read(0, buf, BUFSIZE-1)) < 0) {
      perror("reading from stdin");
      exit(5);
    } else {
      if (write(comm_sock, buf, bytes_read) < 0)  {
	perror("writing on stream socket");
	exit(6);
      }
    }
  } while (bytes_read > 0);
  
  close(comm_sock);

  return 0;
}