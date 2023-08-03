#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ll_ioctl.h"

int main(int argc, const char* argv[])
{
	u_long answer;
	int dev = open("/dev/lldev", O_RDONLY);

	if (dev == -1)
	{
		printf("Opening was not possible!\n");
		return EXIT_FAILURE;
	}

	ioctl(dev, SUM_NODES, &answer);
	printf("The result is = %lu\n", answer);

	ioctl(dev, DELL_NODE, &answer);
	printf("The answer is %lu\n", answer);

	ioctl(dev, SUM_NODES, &answer);
	printf("The result is = %lu\n", answer);


	close(dev);
	return 0;
}

