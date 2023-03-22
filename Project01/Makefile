SRC=./src
HDRS=./headers
OBJS=./objs
LNKS=./links
KREL=$(shell uname -r)
PWD=$(shell pwd)
CMDS=sleep uname ls cat lsmod uptime mkdir chown

all: commands
	gcc ${SRC}/main.c ${OBJS}/commands_${KREL}.o -o ${OBJS}/bin
	$(foreach cmd, $(CMDS), ln -s ${PWD}/objs/bin ${LNKS}/$(cmd);)

commands: ${SRC}/commands.c ${HDRS}/commands.h
	gcc -c ${SRC}/commands.c -o ${OBJS}/commands_${KREL}.o


clean:
	rm ${OBJS}/* &> /dev/null
	rm ${LNKS}/* &> /dev/null