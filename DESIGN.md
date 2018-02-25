# DESIGN SPECIFICATIONS FOR AMAZING CHALLENGE
## Group 8: Rachel Martin, Ross Guju, Rafael Brantley, Steven Karson

### AMStartup script
1. Input: The only inputs are the following three arguments, listed in any order:
	-n nAvatars: (int) the number of Avatars in the maze
	-d Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
	-h Hostname: (char \*) the hostname of the server. Our server will be running on flume.cs.dartmouth.edu

2. Output: The script creates a log file named `Amazing_$USER_N_D.log`
		$USER is the current userid, 
		N is the value of nAvatars,
		D is the value of Difficulty
	The first line of the file contains $USER, the MazePort, and the date & time. 

3. Data Flow: 
<<<<<<< HEAD
	1. AMStartup parse the parameters, calls startup.c with th proper values, and initializes _avatarClient_ the proper number of times
	2. _startup.c_ connects to the server and recieves an `AM_INIT_OK` message with the MazePort
	3. _avatarClient.c_ send each requested move to the server and recieves the state of the maze from the server
=======
	1. AMStartup parses the parameters and calls amazingClient.c with the proper values
	2. _amazingClient.c_ first calls startup.c 
	3. _startup.c_ connects to the server with an AM_Init message and receives an AM_INIT_OK message and returns the message to amazingClient
	4. _amazingClient.c_ then creates each thread with the proper port number which each send an AM_AVATAR_READY message
		When AM_AVATAR_TURN is recieved, the proper thread sends an AM_AVATAR_MOVE message according to our heuristic.
>>>>>>> updates

4. Data Structures: 
	We utilize all the data structures defined in amazing.h.
	We will also have an array of all the threads, an array of the positions of the avatars and a graph that represents the maze.
		The graph will originally have edges between all orthogonal nodes and the edges will be deleted as we find there is a wall.
		After every turn is requested, we compare the array of positions to the updated positions sent in AM_AVATAR_TURN and see if the move was accepted. We then update our client array of positions.

5. Pseudo Code: Pseudo code description of the module.
	Arguments are parsed and nAvatars, Difficulty and Hostname are populated
<<<<<<< HEAD
		`Amazing_$USER_N_D.log` is started with the first line containing $USER, the MazePort, and the date & time
		avatarclient.c is started with the parameters:
			AvatarId 
			nAvatars 
			Difficulty 
			Host name of the server
			MazePort 
			Filename of the log 
			`AM_INIT` is sent to the server with the game specs
				`AM_INIT_OK` is sent back
=======
	Amazing_$USER_N_D.log is started with the first line containing $USER, the MazePort, and the date & time		
	amazingClient.c is started with the parameters:
			Hostname 
			17235 
			nAvatars 
			Difficulty 
			log file name
		AM_connect is called and returns the AM_INIT_OK message
			AM_INIT is sent to the server with the game specs
				AM_INIT_OK is sent back
>>>>>>> updates
			N processes are started
			Each process send `AM_AVATAR_READY`
			Until an error or `AM_MAZE_SOLVED` is returned,
				`AM_AVATAR_TURN` is recieved
				The new positions of each avatar is examined
				Succes or failure of the move is logged
				The avatar specified by `TurnID` sends `AM_AVATAR_MOVE`
				The requested move is appended to the log file
			All data structures are freed and MazePort is closed
