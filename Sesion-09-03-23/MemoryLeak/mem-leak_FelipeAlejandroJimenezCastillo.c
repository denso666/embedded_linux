#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void main(){
	char *path = NULL;

	while(1) {
		path = malloc(100);
		free(path); // free first malloc

		path = strdup("STRING");
		free(path); // free strdup-malloc
		path = NULL;
	}
}
