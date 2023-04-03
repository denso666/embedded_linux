#!/bin/bash
ARGS=($@)
OLD_PATH=$(pwd)
#
# Helpers
#
function help {
    echo "patch - apply a diff file to an original"
    echo "Usage: patch -patchfile -path"
    printf "\tfile\n\t\tPatch file with relative or absolute path, use .patch extension\n"
    printf "\tpath\n\t\tDestination repository path\n"
    exit 0
}

#
# Patch
#
if [ $# -eq 2 ]; then
    # File verification 
    if [[ -f "${ARGS[0]}" ]]; then
        PATCHFILE=$(realpath ${ARGS[0]})

        # Path verification
        if [[ -d "${ARGS[1]}" ]]; then
            PATCHPATH=$(realpath ${ARGS[1]})

            # Change
            sed -i "s/^rename to \w\+\(\.[A-Za-z]{3}\)\?$/&_JCFA/" $PATCHFILE

            # Apply
            patch -d $PATCHPATH < $PATCHFILE
            echo "patch: $PATCHFILE: correctly applied"
            cd $OLD_PATH
            exit 0
        else
            echo "patch: path not found"
            exit 1
        fi
    else
        echo "patch: patchfile not found"
        exit 1
    fi
elif [[ ${args[0]} == "-h" ]] || [[ ${args[0]} == "--help" ]]; then
    help
    exit 0
else
    echo "patch: too few arguments"
    echo "Try 'patch --help' for more information."
    exit 1
fi

