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
	1. AMStartup parse the parameters, calls startup.c with th proper values, and initializes _avatarClient_ the proper number of times
	2. _startup.c_ connects to the server and recieves an `AM_INIT_OK` message with the MazePort
	3. _avatarClient.c_ send each requested move to the server and recieves the state of the maze from the server

4. Data Structures: 
	`struct hostent` - used to represent an entry in the hosts database
	`struct sockaddr_in` - holds the ip address of the host
	`struct sockaddr` - a generic descriptor for any kind of socket operation
	`struct XYPos` - two variables of type uint32_t which are the position of an avatar int he maze
	`struct Avatar` - holds the ******************* and the position of the avatar 
	`struct AM_Message` - used to send uniform messages to the server. Contains a uint32_t that is the type of message and one of a variety of structs listed below:
		struct init;
		struct init_ok;
		struct init_failed;
		struct avatar_ready;
		struct avatar_turn;
		struct avatar_move;
		struct maze_solved;
		struct unknown_msg_type;

5. Pseudo Code: Pseudo code description of the module.
	Arguments are parsed and nAvatars, Difficulty and Hostname are populated
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
			N processes are started
			Each process send `AM_AVATAR_READY`
			Until an error or `AM_MAZE_SOLVED` is returned,
				`AM_AVATAR_TURN` is recieved
				The new positions of each avatar is examined
				Succes or failure of the move is logged
				The avatar specified by `TurnID` sends `AM_AVATAR_MOVE`
				The requested move is appended to the log file
			All data structures are freed and MazePort is closed
