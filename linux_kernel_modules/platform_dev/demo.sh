#!/bin/sh

LED1="/proc/ext-0"
LED2="/proc/ext-1"
LED3="/proc/ext-2"
LED4="/proc/ext-3"
ON=1
OFF=0

function set_state() {
    echo $1 > "$2"
    if [ $? -ne 0 ]; then
        echo "An error was occured: $2"
        exit 1
    fi
}

set_state $OFF $LED1
set_state $OFF $LED2
set_state $OFF $LED3
set_state $OFF $LED4

i=0
while [ $i -lt 10000 ]; do
    set_state $ON $LED1
    usleep 200000

    set_state $ON $LED2
    usleep 200000

    set_state $ON $LED3
    usleep 200000

    set_state $ON $LED4
    set_state $OFF $LED1
    usleep 200000

    set_state $OFF $LED2
    usleep 200000

    set_state $OFF $LED3
    usleep 200000

    set_state $ON $LED3
    usleep 200000

    set_state $ON $LED2
    usleep 200000

    set_state $OFF $LED4
    set_state $ON $LED1
    usleep 200000

    set_state $OFF $LED3
    usleep 200000

    set_state $OFF $LED2
done

exit 0
