#!/bin/bash

# Variables
args=( $@ )
SECS=$(date +%s)
ABS_PATH=$(pwd)

if [ $# -gt 0 ]; then
    find ${args[0]} -type f &> /tmp/${SECS}_file
   
    if [[ $(cat /tmp/${SECS}_file) = ${args[0]} ]]; then
		exit 0
	else
		exit 1
	fi
fi

exit 1
