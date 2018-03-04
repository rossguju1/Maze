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

//I have included xyPosition coordinate into the MazeNode to replace the i, j indexes when building the graph, also it is convient to include
//the xyPosition structure in the MazeNode structure

typedef struct MazeNode
{
	int west;
    int east; 
    int north;
    int south;
} MazeNode_t;

// Okay, this is where things get dicey. T

typedef struct MazeMap
{
	int width;
	int height;
	// I constructed MazeNode **map like the hashtable. map could also be a ***map, but 
	// I avoided additional pointers
	MazeNode_t** map;   //0 path, 1 wall
} MazeMap_t;

static MazeNode_t * MazeNode_new();

MazeMap_t *initMazeMap(int width, int height)
{	

    if (width < 1 || height < 1) {
        fprintf(stderr, "not good");
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

void setMapWall(MazeMap_t* map, int pos, char* dir)
{
  if(strcmp(dir, "north") == 0) {
    map->map[pos]->north = 1;
  }
  if(strcmp(dir, "south") == 0) {
    map->map[pos]->south = 1;
  }
  if(strcmp(dir, "east") == 0) {
    map->map[pos]->east = 1;
  }
  if(strcmp(dir, "west") == 0) {
    map->map[pos]->west = 1;
  }
}
  
int getMapWall(MazeMap_t* map, int pos, char* dir) 
{
  if(strcmp(dir, "north") == 0) {
    return (map->map[pos]->north);
  }
  if(strcmp(dir, "south") == 0) {
    return (map->map[pos]->south);
  }
  if(strcmp(dir, "east") == 0) {
    return (map->map[pos]->east);
  }
  if(strcmp(dir, "west") == 0) {
    return(map->map[pos]->west);
  }
  else return -1;
}