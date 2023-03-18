#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

int valid_time_interval(const char* nptr);

int main(int argc, char const *argv[])
{
	const char* command = argv[1];
	if (argc < 2) printf("No command provide :(\n");
	else
	{
		// sleep 	-> OK
		if (!strcmp(command, "sleep"))
		{
			// not time interval provided
			if (argc == 2)
			{
				printf("sleep: missing operan\n");
				exit(1);
			}
			else
			{
				// verify time intervals
				int status;
				int invalid_intervals = 0;
				for (int i=2; i<argc; i++)
				{
					status = valid_time_interval(argv[i]);
					if (status == 2)
					{
						printf("sleep: invalid time interval '%s'\n", argv[i]);
						invalid_intervals = 1;
					}
				}
				// invalid intervals provided, exit with failure
				if (invalid_intervals) exit(1);
				// valid intervals provided
				else
				{
					for (int i=2; i<argc; i++) sleep(atoi(argv[i]));
				}
			}
		}
		// uname 	-> 
		else if (!strcmp(command, "uname"))
		{
			struct utsname buf;
			if (!uname(&buf))
			{
				printf("Name\t\t%s\n", buf.sysname);
				printf("Release\t\t%s\n", buf.release);
				printf("Version\t\t%s\n", buf.version);
				printf("Proccesor\t%s\n", buf.machine);
			}
			else
			{
				printf("something goes wrong with 'uname'\n");
				exit(1);
			}
		}
		// ls 		->
		else if (!strcmp(command, "ls"))
		{
			const char* path = "./";

			// specific path provided
			if (argc > 2)
			{
				path = argv[2];
			}

  			DIR *d;
  			struct dirent *dir;
  			d = opendir(path);
  			if (d)
			{	
    			while ((dir = readdir(d)) != NULL) {
      				printf("%s\n", dir->d_name);
    			}
    			closedir(d);
  			}
			else
			{
				printf("ls: cannot access '%s': No such file or directory\n", path);
				exit(1);
			}
		}
		// cat		-> OK // optimize code
		else if (!strcmp(command, "cat"))
		{
			// available files provided
			if (argc >= 3)
			{
				int fd;
				int i = 2;
				int size;
				struct stat st;
				char* buf;
				do
				{
					// child execute command with their own exit status
					if (fork() > 0)
					{
						// exist file or directory?
						if (stat(argv[i], &st) != -1)
						{
							if ((st.st_mode & S_IFMT) == S_IFDIR)
							{
								printf("cat: %s: Is a directory\n", argv[i]);
								exit(1); // error: is a directory
							}
							else
							{
								size = st.st_size;
								fd = open(argv[i], O_RDONLY);
								buf = (char*)malloc(sizeof(char)*size);
								read(fd, (void*)buf, size);
								close(fd);
								write(0, buf, size);
								free(buf);
								exit(0); // all ok
							}
						}
						else
						{
							printf("cat: %s: No such file or directory\n", argv[i]);
							exit(1); // error: file or directory not found
						}
					}
				}
				while ( ++i < argc );
			}
			else
			{
				printf("cat: Not provided file or directory\n");
			}
		}
		// lsmod 	-> OK // sanitize code
		else if (!strcmp(command, "lsmod"))
		{
			if (argc > 2)
			{
				printf("Usage: lsmod\n");
				exit(1);
			}
			else
			{
				// need to read /proc/modules
				int fd = open("/proc/modules", O_RDONLY);
				if (fd)
				{
					char* buf = malloc(1);
					while (read(fd, buf, 1))
					{
						printf("%s", buf);
					}
					close(fd);
					free(buf);
				}
				// error on fs /proc/modules
				else
				{
					printf("something goes wrong with 'lsmod'\n");
					exit(1);
				}
			}
		}
	}
	return 0;
}
int valid_time_interval(const char* nptr)
{
	if (nptr != NULL)
	{
		int i = 0;
		while (nptr[i] != '\0')
		{
			// is a non valid numeric character
			if (!isdigit(nptr[i])) return 2;
			i++;
		}
		return 0; // valid number
	}
	return 1; // null ptr provided
}