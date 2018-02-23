#!/bin/bash
#
# AMStartup.sh - parses arguments, connects to the server, and initializes n Avatars 
#
# usage: AMStartup.sh -n nAvatars -d Difficulty -h Hostname
#	where nAvatars is an int
#	Difficulty is a int
#	Hostname is a char*
#
# input: only command line arguments
# output: a log file with that holds each requested move and whether or no the move succeeded
#
# Amazing Challenge
# Group 8, February 21, 2018

options_found=0

while getopts :n:d:h: opt; do
  case $opt in
  	n) 
		nAvatars=$OPTARG
		((options_found++))
		;;
	d)
		Difficulty=$OPTARG
		((options_found++))
		;;
	h)
		Hostname=$OPTARG
		((options_found++))
		;;
	\?) 
		echo "Invalid option: -$OPTARG"  >&2
		exit 1
		;;
	:) 
		echo "Option -$OPTARG requires an argument." >&2
		exit 1
		;;
	esac
done

if [ $options_found != 3 ]; then
  echo "The 3 required options were not found. Only $options_found were found" >&2
  exit 1
fi

./startup $Hostname 17235 $nAvatars $Difficulty
MazePort=$?

if [[ MazePort -gt 0 && MazePort -lt 7 ]]; then 
	echo "Error initializing maze." >&2
	exit 2
fi

echo "Maze port number is $MazePort"



