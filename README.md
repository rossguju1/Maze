# amazingClient.c

Rachel Martin, Steven Karson, Ross Guju, Rafael Brantley, March 2018



### Usage

To run,
`./AMStartup.sh -n <number of avatars> -d <difficulty> -h <host>`



### Compilation

Type `make` in the `cs50W18-8` directory

For manual compilation of `amazingClient`,

If compiling on one of Dartmouth Servers,
`mygcc amazingClient.c localMaze.c startup.c -o amazingClient -lpthread`

If compiling on a computer without the `mygcc` alias,
`gcc -Wall -pedantic -std=c11 amazingClient.c localMaze.c startup.c -o amazingClient -lpthread`



### Editing

If using the _vim_ text editor, please add the following configurations to _.vimrc_ to view the source code clearly:

```
set tabstop=2
set shiftwidth=2
```
