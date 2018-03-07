# The Maze Challenge

## Rachel Martin, Steven Karson, Ross Guju, Rafael Brantley, March 2018

### amazingClient

_amazingClient_ communicates with the CS 50 to solve the Amazing Maze Challenge with varying difficulties and numbers of avatars.

_AMStartup.sh_ parses parameters and calls _amazingClient.c_
_amazingClient.c_ manages the log file runs a thread for each avatar, which implements the right hand rule
_startup.c_ makes the initial connection to the server and gets our MazePort and maze dimensions.
	This is called by _amazingClient.c_ through the method _AMconnect()_
_localMaze.c_ contains the client-side repressentation of the maze

### Usage

To run,
`./AMStartup.sh -n <number of avatars> -d <difficulty> -h <host>`

To run with valgrind,
`./amazingClient flume.cs.dartmouth.edu 17235 2 0 Am_rmartin_2_0.log rmartin`

### Compilation

To compile, type `make` in the `cs50W18-8` directory

For manual compilation of `amazingClient`,

	If compiling on one of Dartmouth Servers,
	`mygcc amazingClient.c localMaze.c startup.c -o amazingClient -lpthread`

	If compiling on a computer without the `mygcc` alias,
	`gcc -Wall -pedantic -std=c11 amazingClient.c localMaze.c startup.c -o amazingClient -lpthread`

To clean up, run `make clean`.

### Editing

If using the _vim_ text editor, please add the following configurations to _.vimrc_ to view the source code clearly:

```
set tabstop=2
set shiftwidth=2
```

