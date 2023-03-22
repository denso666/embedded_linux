SRC=./src
HDRS=./headers
OBJS=./objs
KREL=$(shell uname -r)


all: commands
	gcc ${SRC}/main.c ${OBJS}/commands_${KREL}.o -o ${OBJS}/bin

commands: ${SRC}/commands.c ${HDRS}/commands.h
	gcc -c ${SRC}/commands.c -o ${OBJS}/commands_${KREL}.o

clean:
	rm ${OBJS}/*