#include "../headers/commands.h"

int main(int argc, char const *argv[])
{
	pid_t child_id;
	int status;
	const char* command = argv[0];
	// sleep 	-> OK
	if (!strcmp(command, "./sleep"))
	{
		// not enough arguments
		if (argc == 1)
		{
			printf("sleep: missing operan\n");
			exit(1);
		}
		else __sleep__(argc, argv);
	}
	// uname 	-> OK
	else if (!strcmp(command, "./uname"))
	{
		// extra arguments
		if (argc > 1)
		{
			printf("uname: extra operand '%s'\n", argv[1]);
			printf("Usage: uname\n");
			exit(1);
		}
		else __uname__();
	}
	// ls 		-> VERIFY
	else if (!strcmp(command, "./ls"))
	{
		if (argc == 1) __ls__("./");
		else
		{
			for (int i=1; i<argc; i++)
			{
				if ((child_id = fork()) == 0) __ls__(argv[i]);
				else if (child_id < 0)
				{
					perror("fork");
					exit(1);
				}
				else waitpid(child_id, &status, 0);
			}
		}		
	}
	// cat		-> OK
	else if (!strcmp(command, "./cat"))
	{
		// not enough arguments
		if (argc == 1)
		{
			printf("cat: not provided file or directory\n");
			exit(1);
		}
		else
		{
			for (int i=1; i<argc; i++)
			{
				if ((child_id = fork()) == 0) __cat__(argv[i]);
				else if (child_id < 0)
				{
					perror("fork");
					exit(1);
				}
				else waitpid(child_id, &status, 0);
			}
		}
	}
	// lsmod 	-> OK
	else if (!strcmp(command, "./lsmod"))
	{
		// extra arguments
		if (argc > 1)
		{
			printf("lsmod: extra operand '%s'\n", argv[2]);
			printf("Usage: lsmod\n");
			exit(1);
		}
		else __lsmod__();
	}
	// uptime	-> OK
	else if (!strcmp(command, "./uptime"))
	{
		// extra arguments
		if (argc > 1)
		{
			printf("uptime: extra operand '%s'\n", argv[2]);
			printf("Usage: uptime\n");
			exit(1);
		}
		else __uptime__();
	}
	// mkdir	-> OK
	else if (!strcmp(command, "./mkdir"))
	{
		// not enough arguments
		if (argc == 1)
		{
			printf("mkdir: missing operand\n");
			exit(1);
		}
		else
		{
			for (int i = 1; i<argc; i++)
			{
				if ((child_id = fork()) == 0) __mkdir__(argv[i]);
				else if (child_id < 0)
				{
					perror("fork");
					exit(1);
				}
				else waitpid(child_id, &status, 0);
			}
		}
	}
	// chown	-> OK
	else if (!strcmp(command, "./chown"))
	{
		// not enough or extra arguments
		if (argc != 4)
		{
			printf("chown: missing or extra operand\n");
			printf("Usage: chown -path -group -user\n");
			exit(1);
		}
		else __chown__(argv[1], argv[2], argv[3]);
	}
	// chmod 	->
	else if (!strcmp(command, "./chmod"))
	{
		if (argc != 3)
		{
			printf("chmod: missing operand\n");
			printf("Usage: chmod -path -permissions\n");
			exit(1);			
		}
		else __chmod__(argv[1], argv[2]);
	}
	// command not found
	else
	{
		printf("'%s' command not found\n", argv[0]);
		exit(1);
	}
	return 0;
}
