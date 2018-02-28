
/*
 *	startup.h - a header file for connecting to the maze-server.
 *
 *	`startup` creates a socket and connects to the server hosting the maze.
 *	It then sends an AM_Message of type AM_INIT.
 *	If the AM_INIT message was well-formed, and contained the correct values,
 *	the server should then respond with its own AM_Message, AM_INIT_OK,
 *	which `startup` will return by value.
 *
 */

#ifndef __STARTUP_H
#define __STARTUP_H


typedef struct AM_Message AM_Message;


/*
 *	Sends AM_INIT to the server.
 *	@hostname: the hostname of the server
 *	@port: the port on which we want to connect to the server
 *	@numAva: the number of avatars with which we will play
 *	@diff: the difficulty on which we will play
 *
 *	Returns the AM_INIT_OK message sent by the server.
 */
AM_Message* AM_connect(char *hostname, int port, int numAva, int diff);


#endif
