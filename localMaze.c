#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>                     
#include <stdint.h>             
#include <netinet/in.h>
#include <sys/socket.h>          
#include <arpa/inet.h>
#include <string.h>                        
#include <pthread.h>     
#include "localMaze.h"

typedef struct MazeNode
{
    //int: 0 path, 1 wall
    int west;
    int east;
    int north;
    int south;
} MazeNode_t;

typedef struct MazeMap
{
	int width;
	int height;
	// 1-D array of pointers to "MazeNode_t"s
	MazeNode_t** map;   
} MazeMap_t;

static MazeNode_t* MazeNode_new();

MazeMap_t *initMazeMap(int width, int height)
{	

    if (width < 1 || height < 1) {
        fprintf(stderr, "Improper dimensions");
        return NULL;
    }

    // memmory for the MazeMap structure
    MazeMap_t *MazeMap = malloc(sizeof( MazeMap_t));
    if(MazeMap == NULL) {
      return NULL; // error allocating memory
    } else {
      MazeMap->width = width;
      MazeMap->height = height;
      MazeMap->map = calloc(width*height, sizeof(MazeNode_t*));
      for(int i = 0; i < width*height; i++) {
        MazeMap->map[i] = MazeNode_new();
        if(i >= 0 && i < width) {
          MazeMap->map[i]->north = 1;
        }
        if(i >= (width*height)-width) {
          MazeMap->map[i]->south = 1;
        }
        if(i%width == 0) {
          MazeMap->map[i]->west = 1;
        }
        if(i%width == (width -1)) {
          MazeMap->map[i]->east = 1;
        }
      }
      return MazeMap;
    }
}

static MazeNode_t * // not visible outside this file
MazeNode_new()
{
  MazeNode_t *node = malloc(sizeof(MazeNode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    node->west = 0;
    node->north = 0;
    node->south = 0;
    node->east = 0;
    return node;
  }
}

int getWidth(MazeMap_t* map) 
{
  return map->width;
}

int getHeight(MazeMap_t* map) 
{
  return map->height;
}

void setMapWall(MazeMap_t* maze, int pos, int dir)
{
  if ( dir == 0) {
    maze->map[pos]->north = 1;
  }
  if (dir == 1) {
    maze->map[pos]->east = 1;
  }
  if(dir == 2) {
    maze->map[pos]->south = 1;
  }
  if(dir == 3) {
    maze->map[pos]->west = 1;
  }
}
  
int getMapWall(MazeMap_t* maze, int pos, int dir) 
{
  if(dir == 0) {
    return (maze->map[pos]->north);
  }
  if(dir == 1) {
    return (maze->map[pos]->east);
  }
  if(dir == 2) {
    return (maze->map[pos]->south);
  }
  if(dir == 3) {
    return(maze->map[pos]->west);
  }
  else return -1;
}

void deleteMaze(MazeMap_t* map)
{
  if(map != NULL) {
    if (map->map != NULL) {
      for(int i = 0; i < map->width*map->height; i++) {
        if(map->map[i] != NULL) {
          free(map->map[i]);
        }
      }
      free(map->map);
    }
    free(map);                 // finally, free the MazeMap struct
  }
}