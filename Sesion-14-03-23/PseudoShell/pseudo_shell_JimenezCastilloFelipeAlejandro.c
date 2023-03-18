#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void spawn (char*, char**);

int main (int argc, char* argv[]) {
	char* date[] = { "date", NULL };
	char* whoami[] = { "whoami", NULL };
	char* uptime[] = { "uptime", "-p", NULL };
	char* personal[] = { "sudo", "./sus2Ram", NULL };

	if ( argc != 2 ) printf("Invalid arguments size :(\n");
	else {
		pid_t child_pid;
		int status;

		child_pid = fork();
		if ( child_pid == -1 ) {
			printf("fork failure\n");
			return -1;
		}
		else if ( child_pid > 0 ) {
			if ( !strcmp(argv[1], "fecha") ) spawn(date[0], date);
			else if ( !strcmp(argv[1], "quiensoy") ) spawn(whoami[0], whoami);
			else if ( !strcmp(argv[1], "tiempo") ) spawn(uptime[0], uptime);
			else if ( !strcmp(argv[1], "personal") ) spawn(personal[0], personal);
			else printf("Command not found :(\n");
		}
	}
	return 0;
}

void spawn (char* program, char** arg_list) { execvp(program, arg_list); }