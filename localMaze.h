
/*
 *	localMaze.h - a header file for creating a local representation of the Amazing Maze
 *
 *	Each point on the map's grid is represented by an internal `struct MapNode`.
 *	The Map is simply a list of these Nodes.
 *	The Nodes in the map are ordered by index, counting left-to-right, then top-to-bottom.
 *	Each pair of orthogonally adjacent nodes can have a path or a wall in between.
 *		These paths/walls can be get/set with the methods in this module.
 */


#ifndef __localMaze_H
#define __localMaze_H


typedef struct MazeMap MazeMap_t;


/*
 *	Initializes the map.
 *	Creates a `struct MazeMap` of width `width` and height `height`
 *	All of the orthogonally adjacent nodes are connected.
 *		Except for the nodes on the edges of the map, which will be walled off on their sides touching the edges.
 */
MazeMap_t *initMazeMap(int width, int height);



/* 
 * 	Puts a wall between nodes
 *  The edge is specified by the side `dir` on one node `pos`
 */
void setMapWall(MazeMap_t* map, int pos, int dir);


// getters
int getWidth(MazeMap_t* map);
int getHeight(MazeMap_t* map);
int getMapWall(MazeMap_t* map, int pos, int dir);


// Frees all allocated memory
void deleteMaze(MazeMap_t* map);


#endif
