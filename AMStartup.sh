#!/bin/bash
#
# AMStartup.sh - parses arguments, and calls amazingClient with the proper arguments
# usage: AMStartup.sh -n nAvatars -d Difficulty -h Hostname
#	where nAvatars is an int
#	Difficulty is a int
#	Hostname is a char*
#
# input: only command line arguments
# output: a log file with that holds the output of a maze solution
#
# Amazing Challenge
# Group 8, Rachel Martin, Steven Karson, Ross Guju, Raphael Brantley, February 21, 2018

options_found=0

#checks to make sure each flag and parameter is given

while getopts :n:d:h: opt; do
  case $opt in
  	n) 
		nAvatars=$OPTARG
		if [ "$nAvatars" -lt 2 ]; then
			echo "There must be at least 2 avatars" >&2
			exit 1
		fi
		((options_found++))
		;;
	d)
		Difficulty=$OPTARG
		if [ "$Difficulty" -gt 9 ] || [ "$Difficulty" -lt 0 ]; then
			echo "Difficulty must be between 0 and 9" >&2
			exit 1
		fi
		((options_found++))
		;;
	h)
		Hostname=$OPTARG
		((options_found++))
		;;
	\?) 
		echo "Invalid option: -$OPTARG"  >&2
    echo "usage: AMStartup.sh -n nAvatars -d Difficulty -h Hostname"
		exit 1
		;;
	:) 
		echo "Option -$OPTARG requires an argument." >&2
    echo "usage: AMStartup.sh -n nAvatars -d Difficulty -h Hostname"
		exit 1
		;;
	esac
done

if [ $options_found != 3 ]; then
  echo "The 3 required options were not found. Only $options_found were found" >&2
  echo "usage: AMStartup.sh -n nAvatars -d Difficulty -h Hostname"
  exit 1
fi

# calls amazingClient with the arguments given and the proper name of the log file
./amazingClient $Hostname 17235 $nAvatars $Difficulty "Amazing_""$USER""_""$nAvatars""_""$Difficulty".log $USER

