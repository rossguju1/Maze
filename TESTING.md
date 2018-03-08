# CS50 The Maze Challenge
## Group 8, Rachel Martin, Steven Karson, Ross Guju, Raphael Brantley, 3/6/18

### Unit Testing

To test our code, we first tested to make sure our communication with the server was working as expected.
To do this, we used a unit testing file called [servertest](servertest.c).

We also tested our argument checking by calling the bash script with invalid numbers of avatars, invalid levels, and missing required arguments. Instead of crashing, the program exits gracefully.

In addition, we tested to make sure the code was following the right hand rule by looking at the png's produced by the server. We confirmed this worked properly.

### Testing different levels

We tested our code all the way up to level 7 with 4 avatars and got successs. The log files from these runs can be seen in `successful_log_examples`.
