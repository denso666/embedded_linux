#!/bin/bash

# Variables
args=( $@ )
SECS=$(date +%s)
ABS_PATH=$(pwd)

#
# Helpers
#
function help {
    echo "patch - apply a diff file to an original"
    echo "Usage: patch -file -path"
    printf "\tfile\n\t\tPatch file with relative or absolute path, use .patch extension\n"
    printf "\tpath\n\t\tDestination repository path\n"
}


if [ $# -gt 0 ]; then
    # Help
    if [[ ${args[0]} == "-h" ]] || [[ ${args[0]} == "--help" ]]; then
        help
        exit 0
    fi
    

    find ${args[0]} -type f &> /tmp/${SECS}_file

    # File verification 
    if [[ $(cat /tmp/${SECS}_file) != ${args[0]} ]]; then
        rm /tmp/${SECS}_file &> /dev/null
        echo "patch: file not found"
        exit 1
    fi
    
    # Path verification
    # if [[ ]]
    exit 0
else
    echo "patch: not .patch file provided"
    echo "Use patch -h [--help] to see more information"
    exit 1
fi
