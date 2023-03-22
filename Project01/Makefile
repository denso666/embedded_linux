SRC=./src
HDRS=./headers
OBJS=./objs
LNKS=./links
KREL=$(shell uname -r)
PWD=$(shell pwd)

all: commands
	gcc ${SRC}/main.c ${OBJS}/commands_${KREL}.o -o ${OBJS}/bin
	ln -s ${PWD}/objs/bin ${LNKS}/sleep
	ln -s ${PWD}/objs/bin ${LNKS}/uname
	ln -s ${PWD}/objs/bin ${LNKS}/ls
	ln -s ${PWD}/objs/bin ${LNKS}/cat
	ln -s ${PWD}/objs/bin ${LNKS}/lsmod
	ln -s ${PWD}/objs/bin ${LNKS}/uptime
	ln -s ${PWD}/objs/bin ${LNKS}/mkdir
	ln -s ${PWD}/objs/bin ${LNKS}/chown

commands: ${SRC}/commands.c ${HDRS}/commands.h
	gcc -c ${SRC}/commands.c -o ${OBJS}/commands_${KREL}.o


clean:
	rm ${OBJS}/*
	rm ${LNKS}/*