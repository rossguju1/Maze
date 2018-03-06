# DESIGN SPECIFICATIONS FOR AMAZING CHALLENGE
## Group 8: Rachel Martin, Ross Guju, Rafael Brantley, Steven Karson

1. Input: The only inputs are the following three arguments, listed in any order:
	* -n nAvatars: (int) the number of Avatars in the maze
	* -d Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
	* -h Hostname: (char \*) the hostname of the server. Our server will be running on flume.cs.dartmouth.edu

2. Output: The script creates a log file named `Amazing_$USER_N_D.log`
		* $USER is the current userid, 
		* N is the value of nAvatars,
		* D is the value of Difficulty
	* The first line of the file contains $USER, the MazePort, and the date & time. 
  * Subsequent lines are move requests and direction of request, whether the move was accepted
  * The final line indicates the success or failure of the maze solution

3. Data Flow: 
	1. `AMStartup.sh` parses the parameters and calls `amazingClient.c` with the proper values
	2. `amazingClient.c` first calls `startup.c`
	3. `startup.c` connects to the server with an `AM_Init` message and receives an `AM_INIT_OK` message and returns the message to amazingClient
	4. `amazingClient.c` then creates each thread with the proper port number which each send an `AM_AVATAR_READY` message
		* When `AM_AVATAR_TURN` is recieved, the proper thread sends an AM_AVATAR_MOVE message according to our heuristic.

4. Data Structures: 
	We utilize a subset of the data structures defined in `amazing.h`
	We also have a global array of all the threads, and a global `mazeMap_t` graph that represents the maze.
		* mazeMap_t - The graph will be a 1-D array of size = (MazeWidth * MazeHeight) of pointers to mazeNodes, each of which hold an `int` for each cardinal direction. The integer is 0 if there is a path in that direction and 1 if there is a wall. We originally have no walls between orthogonal nodes. We add walls as they are discovered.

5. Pseudo Code:
	* Arguments are parsed and nAvatars, Difficulty and Hostname are populated
	* `Amazing_$USER_N_D.log` is started with the first line containing $USER, the MazePort, and the date & time		
	* amazingClient.c is started with the parameters:
		* Hostname 
		* 17235 
		* nAvatars 
		* Difficulty 
		* log file name
	* `AM_connect` is called and returns a `AM_INIT_OK` message
	* A graph is initialized with the number of nodes equal to (MazeWidth * MazeHeight) and all othrogonal edges initialized.
  * log file is created
	* An array of threads for the avatars is created, each thread running `run_thread`
	* A socket is created for each and connects to the server and sends the ready message
	* Initial `AM_AVATAR_TURN` is parsed 
	* While not yet solved and moves < `AM_MAX_MOVES`
		* For each avatar thread
			* Decide on a move with our algorithm
			* Send an AM_AVATAR_MOVE message
			* Append to the log file the requested move
	  * If an error or solved message occurred, exit thread with the proper status
  * Append to the log the exit status of the maze
	* Close sockets and clean up data structures

	Our maze solving algorithm,
	* Check if the avatar is in the center of the graph.
	* If so, request a null move,
	* If not, 
		* set `desiredDir` to the right of the direction the avatar is facing, `dir`
		* Check if the last move of the avatar was accepted
      * If it was not, set a wall in that direction, and update `desiredDir` counter-clockwise, until a direction with no known wall
			* If there is no wall,
				* update position of avatar, `pos` to reflect current position and update `dir`
    * send move message
