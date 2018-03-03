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
#include <localMaze.h>    

// The avatar structure should have some kind of x-y position variable like xyPosition so that we can keep track of its x-y coordinates

typedef struct xyPosition
{
	uint32_t x;
	uint32_t y;
} xyPosition_t;


//I have included xyPosition coordinate into the MazeNode to replace the i, j indexes when building the graph, also it is convient to include
//the xyPosition structure in the MazeNode structure

typedef struct MazeNode
{
	int west;
    int east; 
    int north;
    int south;
  xyPosition coordinate;
} MazeNode_t;

// Okay, this is where things get dicey. T

typedef struct MazeMap
{
	int width;
	int height;
	// I constructed MazeNode **map like the hashtable. map could also be a ***map, but 
	// I avoided additional pointers
	struct MazeNode **map;
} MazeMap_t;


MazeMap_t *initMazeMap(int width, int height)
{	
	int i, j;

    if (width < 1 || height < 1) {
        fprintf(stderr, "not good");
        return NULL;
    }


    // memmory for the MazeMap structure
    MazeMap_t *MazeMap = calloc(1, sizeof( MazeMap_t ));

    // memory for each column which is part of the actual map.
    MazeMap->map = calloc(width, sizeof( MazeNode_t ));


    // you can get rid of this 
    map = MazeMap->map;


    // The x- coordinate is the width of the graph
    for ( i = 0; i < width; i++) {
    	//the height is the y-coordinate of the graph
        map[i] = calloc(height, sizeof( MazeNode_t ));
        for (int j = 0; j < height; j++) {
         

         //NOTE 0 for unknown, 1 for no wall, -1 for a wall from here on out
         //HOWEVER, You can easily get rid of the 'unknown for 0' and build the entire 
         //graph as walls. This would only take a few changes.


            map->north = 0;
            map->south = 0;
            map->west = 0;
            map->east = 0;
            

            // Build the outside walls of graph
            if (i = 0) { 
            map->west = -1; 
        	}  if (i = width - 1) {
             map->east = -1; 
         	} if (j = 0) { 
            map->north = -1; 
        	} if (j = height -1) { 
        	map->south = -1; 
        	} 
                 

            // set x-y coordinates     
            map->coordinate.x = i;
            map->coordinate.y = j;
               
        }
    }
    MazeMap->width = width;
    MazeMap->height = height;
    return MazeMap;
}

//NOTE  0 for unknown, 1 for no wall, -1 for a wall

// CHECK function returns which direction avatar should go: right hand rule
// 
// This is the right hand rule algorithm. 
// NOTE The avatar structure should be passed as input through this function:
// 		Moreover, the input should look along the lines of 
// 		Check(MazeMap_t *MazeMap, Avatar_t Avatar)
// 		where Avatar_t Avatar structure would look something like this:
// 		typedef struct Avatar {
// 		struct xyPosition coordinate;
// 		int Direction; (Or OldDirection)
// 		....
// 		other parameters
// 		....
// 		
// 		} Avatar_t;


//NOTE I wasn't 100% sure on my use of pointers and -> 
//     We can very easily not use so many pointers but I stuck with them because of consistency

int Check(MazeMap_t *MazeMap, int OldDirection, xyPosition_t *coordinate) 
{


	// CurrentNode points to Avatar_t Avatar xyPosition coordinates
  MazeNode_t *CurrentNode = MazeMap->map[coordinate->x][coordinate->y];

  // The New Direction is set to GoBack() which is the function that checks if the avatar has reached
  // a dead end. Go check out GoBack() before continuing
  int NewDirection = GoBack(MazeMap, coordinate);
  // If GoBack() returns 1, which implies that the avatar can move
  if(NewDirection = 1) {
    
    // Now, we will follow the right hand rule and check the Wall status' around the avatar
    // If the check-functions return 1, this means that there is no wall. 
    if (NewDirection = CheckRight(MazeMap, OldDirection, coordinate) > 0) {
    	return NewDirection;

    } else if (NewDirection = CheckBack(MazeMap, OldDirection, coordinate) > 0) {
    	return NewDirection;

    } else if (NewDirection = CheckLeft(MazeMap, OldDirection, coordinate) > 0) {
    	return NewDirection;

    } else if (NewDirection = CheckFront(MazeMap, OldDirection, coordinate) > 0) {
    	return NewDirection;
    } else if (NewDirection == -1){ // if all of those cases fail, then there must be a wall; insert wall
            InsertWall(map, coordinate, OldDirection, -1);
          }
      }
       
  return NewDirection;
}


// This is the functution that counts the number of walls around the avatar, if there are 3 walls counted,
// then the avatar is obviously at a Dead End of a path. So we insert a wall and then go back to the previous position
// 

int GoBack(MazeMap_t *MazeMap, xyPosition_t *coordinate)
{
	int NewDirection, NumberOfWalls, x, y;
  x = coordinate->x;
  y = coordinate->y;
	MazeNode_t *CurrentNode = MazeMap->map[x][y];

	NumberOfWalls = 0;
   // these ternary operators are equivalent to if else statements 

	(CurrentNode->north != -1) ? NewDirection = M_NORTH : NumberOfWalls++;
	
	(CurrentNode->south != -1) ? NewDirection = M_SOUTH : NumberOfWalls++;

	(CurrentNode->west != -1) ? NewDirection = M_WEST : NumberOfWalls++;

	(CurrentNode->east != -1) ? NewDirection = M_EAST : NumberOfWalls++;

	// if the number of walls is  more than or equal to 3, then we are at a dead end, Do not proceed forward! and return 
	// the initial direction i.e. x-y coordinates

	if (NumberOfWalls >= 3) {
		InsertWall(coordinate, NewDirection, -1);
		return NewDirection;
	} 
	// Return 1 because we either hit a dead end and we inserted a wall or we are not at a dead end and one of the
	// ternary cases evaluated to true
	return 1;
}

//NOTE the CheckRight, CheckFront, CheckLeft, CheckBack functions are similar in format. Feel free to change them 
//up if you don't like switch-case or some other formatings
//
//For the Check-Functions, OldDirection and xyPosition_t *coordinate input variables SHOULD BE elements of the avatar structure



//NOTE AGAIN: 0 for unknown, 1 for no wall, -1 for a wall

// We don't really need 0 for unknown.

int CheckRight(MazeMap_t *MazeMap, int OldDirection, xyPosition_t *coordinate)
{

 MazeNode_t *CurrentNode = MazeMap->map[coordinate->x][coordinate->y]; 
  switch(OldDirection){


    case M_NORTH:
     if( CurrentNode->east != -1) {
     	return M_EAST;
     } 
     break;
    case M_SOUTH:
      if (CurrentNode->west != -1) {
      	return M_WEST
      } 
    case M_WEST:
      if (CurrentNode->north != -1) {
      	return M_NORTH
      } 
    case M_EAST:
      if (CurrentNode->south != -1) {
       return M_SOUTH; 
      }
    default:
      return -1;
  }
}

int CheckFront(MazeMap_t *MazeMap, int OldDirection, xyPosition_t *coordinate){
  int NewDirection;
  MazeNode_t *CurrentNode = MazeMap->map[coordinate->x][coordinate->y]; 
  switch(OldDirection){
    case M_NORTH:
      if (CurrentNode->north != -1) {
      	return M_NORTH;
      }
    case M_SOUTH:
      if (CurrentNode->south != -1) {
      	return M_SOUTH;
      }
    case M_WEST:
      if (CurrentNode->west != -1) {
      	M_WEST
      } 
    case M_EAST:
      if (CurrentNode->east != -1) {
      	M_EAST
      } 
      default:
      return -1;
  }
}

int CheckLeft(MazeMap_t *MazeMap, int OldDirection, xyPosition_t *coordinate){

  MazeNode_t *CurrentNode = MazeMap->map[coordinate->x][coordinate->y]; 

  switch(OldDirection){
    case M_NORTH:
      if (CurrentNode->west != -1) {
      	return M_WEST;
      } 
    case M_SOUTH:
      if (CurrentNode->east != -1) {
      	return M_EAST;
      	} 
    case M_WEST:
      if (CurrentNode->south != -1) {
      	return M_SOUTH;
      } 
    case M_EAST:
      if (CurrentNode->north != -1) {
      	return M_NORTH;
      }
    default:
      return -1;
  }
}



int CheckBack(MazeMap_t *MazeMap int OldDirection, xyPosition_t *coordinate){
  MazeNode_t *CurrentNode = MazeMap->map[coordinate->x][coordinate->y]; 

  switch(OldDirection){
    case M_NORTH:
      if (CurrentNode->south != -1) {
      	return M_SOUTH;
      } 
    case M_SOUTH:
      if (CurrentNode->north != -1) {
      	return M_NORTH;
      } 
    case M_WEST:
      if (CurrentNode->east != -1) {
      	return M_EAST;
      } 
    case M_EAST:
      if (CurrentNode->west != -1) {
      	return M_WEST;
      } 
    default:
		return -1;

  }
}




// InsertWall, inserts wall for a node and adjacent node
// 
// Wall Status will be -1 which is a wall, when using this function.
// 
// NOTE double check and make sure that I have the x-y coordinates correct for map[x][y].
// 
// I currently have the width nodes representing the x-direction i.e changing columns and the height nodes represent the y direction i.e changing rows

 
void InsertWall(MazeMap_t *MazeMap, xyPosition_t *coordinate, int OldDirection, int WallStatus){



  MazeNode_t *AdjacentNode;

  int x, y;
  x = coordinate->x;
  y = coordinate->y;

    MazeNode_t *CurrentNode = map[x][y];
  switch(OldDirection) {

  case M_NORTH:
    if (y != 0) { 
    // CANT BE IN TOP ROW: update the north wall and the adjacent south wall
      CurrentNode->north = WallStatus;
      AdjacentNode = map[x][y - 1];
      AdjacentNode->south = WallStatus;
    } break;
  case  M_WEST:
    if (x != 0){ 
    //CANT BE IN zeroth COLUMN: update the west wall and the adjacent east wall
      CurrentNode->west = WallStatus;
      AdjacentNode = map[x - 1][y];
      AdjacentNode->east = WallStatus;
    } break;
	
  case M_EAST:
    if (x < map.width - 1){ 
    //CANT BE IN LAST COlumn, update the east wall and the adjacent west wall
      CurrentNode->east = WallStatus;
      AdjacentNode = map[x + 1][y];
      AdjacentNode->west = WallStatus;
    } break;
 	

  case M_SOUTH:
    if (y < map.height - 1){ 
    //CANT BE IN THE BOTTOM ROW, update the south wall and the adjacent north wall
      CurrentNode->south = WallStatus;
      AdjacentNode = map[x][y + 1];
      AdjacentNode->north = WallStatus;
    } break;
	}
	return;
}

// NOTE &some_Mutex, is the pthread that you created for the avatars
// 
// UpdateMap, actually MOVES the avatars

MazeMap_t *UpdateMap(MazeMap_t *MazeMap, int MoveTo, xyPosition_t *Newcoordinate, xyPosition_t *Oldcoordinate)
{
    pthread_mutex_lock(&some_Mutex);
   

   // some error checking but not much. 
    if (MoveTo == -1) {  // avatar's first time moving
        pthread_mutex_unlock(&some_Mutex);
        return MazeMap;
    }
    if (MoveTo != M_NORTH && MoveTo != M_WEST && MoveTo != M_EAST && MoveTo != M_NULL_MOVE && MoveTo != M_SOUTH) {
        fprintf(stderr, "Move Not Allowed\n");
        pthread_mutex_unlock(&some_Mutex);
        return NULL;
    }



    int width = MazeMap->width;
    int height = MazeMap->height;



    // rename so we dont need -> arrow so much

    int NewXPosition = Newcoordinate->x
    int NewYPosition = Newcoordinate->y;
    int OldXPosition = Oldcoordinate->x;
    int OldYPosition = Oldcoordinate->y;


    map = MazeMap->map;
    // These if else statements are situations when the avatar ACTUALLY moved

 		if (OldXPosition == NewXPosition && OldYPosition > NewYPosition) { 
            MazeMap->map[OldXPosition][OldYPosition]->north = 1;
            MazeMap->map[NewXPosition][NewYPosition]->south = 1;
        
        } else if (OldXPosition == NewXPosition && OldYPosition < NewYPosition) {  
            MazeMap->map[OldXPosition][OldYPosition]->south = 1;
            MazeMap->map[NewXPosition][NewYPosition]->north = 1;
        
        } else if (OldXPosition > NewXPosition && OldYPosition == NewYPosition) {  
            MazeMap->map[OldXPosition][OldYPosition]->west = 1;
            MazeMap->map[NewXPosition][NewYPosition]->east = 1;

        } else if (OldXPosition < NewXPosition && OldYPosition == NewYPosition) { 
            MazeMap->map[OldXPosition][OldYPosition]->east = 1;
            MazeMap->map[NewXPosition][NewYPosition]->west = 1;
        } else {
        // This is the situation when the avatar did not move, so we need to put up a wall
        // However, if we are at the edge, there is already a wall, so we use the ternary operators.
        switch (MoveTo) {
            case M_NORTH: 
                MazeMap->map[OldXPosition][OldYPosition]->north = -1;
                (OldYPosition > 0) ? MazeMap->map[OldXPosition][OldYPosition - 1]->south = -1 : break;
            case M_WEST: 
                MazeMap->map[OldXPosition][OldYPosition]->west = -1;
                (OldXPosition > 0) ? MazeMap->map[OldXPosition - 1][OldYPosition]->east = -1 : break;
                    
            case M_EAST: 
                MazeMap->map[OldXPosition][OldYPosition]->east = -1;
                 (OldXPosition < width - 1) ? MazeMap->map[OldXPosition + 1][OldYPosition]->west = -1 : break;
            case M_SOUTH: 
                MazeMap->map[OldXPosition][OldYPosition]->south = -1;
                (OldYPosition < height - 1) ? MazeMap->map[OldXPosition][OldYPosition + 1]->north = -1 : break;
            case M_NULL_MOVE:
            break; 
        }

 }
    pthread_mutex_unlock(&some_Mutex);
    return MazeMap;
}



