# DESIGN SPECIFICATIONS FOR AMAZING CHALLENGE
## Group 8: Rachel Martin, Ross Guju, Rafael Brantley, Steven Karson

### AMStartup script
1. Input: The only inputs are the following three arguments, listed in any order:
	* -n nAvatars: (int) the number of Avatars in the maze
	* -d Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
	* -h Hostname: (char \*) the hostname of the server. Our server will be running on flume.cs.dartmouth.edu

2. Output: The script creates a log file named Amazing_$USER_N_D.log
		* $USER is the current userid, 
		* N is the value of nAvatars,
		* D is the value of Difficulty
	* The first line of the file contains $USER, the MazePort, and the date & time. 

3. Data Flow: 
	1. AMStartup parses the parameters and calls amazingClient.c with the proper values
	2. _amazingClient.c_ first calls startup.c 
	3. _startup.c_ connects to the server with an AM_Init message and receives an AM_INIT_OK message and returns the message to amazingClient
	4. _amazingClient.c_ then creates each thread with the proper port number which each send an AM_AVATAR_READY message
		* When AM_AVATAR_TURN is recieved, the proper thread sends an AM_AVATAR_MOVE message according to our heuristic.

4. Data Structures: 
	We utilize all the data structures defined in amazing.h.
	We will also have an array of all the threads, an array of structs that hold the positions of the avatars and the direction they are facing, and a graph that represents the maze.
		* The graph will be an array of size = (MazeWidth * MazeHeight) and originally will have all edges between orthogonal nodes. The edges will be deleted as we find there is a wall. 
		* After every turn is requested, we compare the array of positions to the updated positions sent in AM_AVATAR_TURN and see if the move was accepted. We then update our client array of positions.

5. Pseudo Code:
	* Arguments are parsed and nAvatars, Difficulty and Hostname are populated
	* Amazing_$USER_N_D.log is started with the first line containing $USER, the MazePort, and the date & time		
	* amazingClient.c is started with the parameters:
		* Hostname 
		* 17235 
		* nAvatars 
		* Difficulty 
		* log file name
	* AM_connect is called and returns a AM_MESSAGE
	* A graph is with the number of nodes equal to (MazeWidth * MazeHeight) and all othrogonal edges initialized.
	* An array of the positions and directions(originally North) for each avatar is created
	* An array of threads for the avatars is created
	* A socket is created for each and connects to the server and sends the ready message
	* Initial AM_AVATAR_TURN is parsed 
	* While not yet solved and moves < AM_MAX_MOVES
		* For each avatar thread
			* Decide on a move with our algorithm
			* Send an AM_AVATAR_MOVE message
			* Append to the log file the requested move
			* Recieve and parse AM_AVATAR_TURN message
				* compare positions to our position array to see if move was valid
				* update local graph by removing an edge if move was invalid
	* If solved, append to the log that it was solved
	* Close sockets and exit

	Our maze solving algorithm,
		* Check if the avatar is in the center of the graph.
		* If so, request no move,
		* If not, 
			* set desiredDirection to the right of the direction the avatar is facing
			* Check if an edge exists in the graph from current position to the the desired direction
				* If so,
					* Send a request to move in that direction
				* If not, update the desiredDirection counter-clockwise around a compass rose (ie, East updates to North)

