


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
AM_Message AM_connect(char *hostname, int port, int numAva, int diff);


#endif
