#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void main( int argc, char **argv ) {
	int fd;
	struct input_event ev;

	fd = open( argv[1], O_RDONLY );
	for ( int i=0; i< 100; ++i ) {
		read( fd, &ev, sizeof(struct input_event) );
		printf( "############## Entry %d ##########\n", i );
		printf( "value=%u\n", ev.value );
		printf( "time=%li\n", ev.time.tv_sec );
		printf( "type=%u\n", ev.type );
		printf( "code=%u\n", ev.code );
	}
	close( fd );

	/* Suspend to RAM
		"In turn, there is only one way to cause the system to go into the Suspend-To-RAM state (write "deep" into /sys/power/mem_sleep and "mem" into /sys/power/state)."[1]

		[1] https://www.kernel.org/doc/Documentation/power/states.txt
	*/
	fd = open( "/sys/power/mem_sleep", O_WRONLY );
	if ( fd > -1 ) {
		write( fd, "deep", 4 );
		close( fd );

		fd = open( "/sys/power/state", O_WRONLY );
		if ( fd > -1 ) {
			write( fd, "mem", 3 );
			close( fd );
		}
		else fprintf( stderr, "Something went wrong with 'open -> /sys/power/state'\n" );
	}
	else fprintf( stderr, "Something went wrong with 'open -> /sys/power/mem_sleep'\n" );
}