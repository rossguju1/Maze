


#ifndef __localMaze_H
#define __localMaze_H


typedef struct MazeMap MazeMap_t;



MazeMap_t *initMazeMap(int width, int height);



// getters
int getWidth(MazeMap_t* map);
int getHeight(MazeMap_t* map);
int getMapWall(MazeMap_t* map, int pos, int dir);



void setMapWall(MazeMap_t* map, int pos, int dir);



void deleteMaze(MazeMap_t* map);


#endif
