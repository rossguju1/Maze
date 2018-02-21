# DESIGN SPECIFICATIONS FOR AMAZING CHALLENGE
## Group 8: Rachel Martin, Ross Guju, Rafael Brantley, Steven Karson

### AMStartup script
1. Input: The only inputs are the following three arguments, listed in any order:
	-n nAvatars: (int) the number of Avatars in the maze
	-d Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
	-h Hostname: (char *) the hostname of the server. Our server will be running on flume.cs.dartmouth.edu
2. Output: The script creates a log file named Amazing_$USER_N_D.log
		$USER is the current userid, 
		N is the value of nAvatars,
		D is the value of Difficulty
	The first line of the file contains $USER, the MazePort, and the date & time. 
3. Data Flow: 
	Arguments are parsed and nAvatars, Difficulty and Hostname are populated
	Amazing_$USER_N_D.log is started with the first line containing $USER, the MazePort, and the date & time
	avatarclient.c is started with the parameters:
		AvatarId 
		nAvatars 
		Difficulty 
		Host name of the server
		MazePort 
		Filename of the log 
		AM_INIT is sent to the server with the game specs
			AM_INIT_OK is sent back
		N processes are started
		Each process send AM_AVATAR_READY
		Until an error or AM_MAZE_SOLVED is returned,
			AM_AVATAR_TURN is recieved
			The new positions of each avatar is examined
			Succes or failure of the move is logged
			The avatar specified by TurnID sends AM_AVATAR_MOVE
			The requested move is appended to the log file
		All data structures are freed and MazePort is closed


4. Data Structures: Major data structures used by the module
5. Pseudo Code: Pseudo code description of the module.