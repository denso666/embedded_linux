#include <sys/utsname.h>
#include <sys/sysinfo.h>
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
#include <time.h>
#include <pwd.h>
#include <grp.h>

int valid_time_interval(const char* nptr);

int main(int argc, char const *argv[])
{
	if (argc < 2) printf("No command provide :(\n");
	else
	{
		const char* command = argv[1];
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
				int total_interval = 0;
				for (int i=2; i<argc; i++)
				{
					status = valid_time_interval(argv[i]);
					if (status == 2)
					{
						printf("sleep: invalid time interval '%s'\n", argv[i]);
						invalid_intervals = 1;
					}
					else
					{
						total_interval += atoi(argv[i]);
					}
				}
				// invalid intervals provided, exit with failure
				if (invalid_intervals) exit(1);
				// valid intervals provided
				else
				{
					sleep(total_interval);
				}
			}
		}
		// uname 	-> OK
		else if (!strcmp(command, "uname"))
		{
			// extra operands
			if (argc > 2)
			{
				printf("uname: extra operand '%s'\n", argv[2]);
				exit(1);
			}
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
		// uptime
		else if (!strcmp(command, "uptime"))
		{
			struct sysinfo s_info;
			if (!sysinfo(&s_info))
			{
				// printf("%ld", ctime(s_info.uptime));
				printf("%ld", s_info.uptime);
			}
			else
			{
				printf("something goes wrong with 'uptime'\n");
				exit(1);
			}
		}
		// mkdir	-> OK
		else if (!strcmp(command, "mkdir"))
		{
			if (argc > 2)
			{
				struct stat st;
				
				if (stat(argv[2], &st) == -1)
				{
					mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					printf("mkdir: created directory '%s'\n", argv[2]);
				}
				else
				{
					printf("mkdir: cannot create directory '%s': File or directory exists\n", argv[2]);
					exit(1);
				}
			}
			else
			{
				printf("mkdir: missing operand\n");
				exit(1);
			}
		}
		// chown	-> OK
		else if (!strcmp(command, "chown"))
		{
			if (argc == 5)
			{
				struct passwd* pwd;
				struct group* grp;
				struct stat st;

				// valid file
				if (stat(argv[2], &st) != 0)
				{
					printf("chown: invalid file: '%s'\n", argv[2]);
					exit(1);
				}
				// valid group
				grp = getgrnam(argv[3]);
				if (!grp)
				{
					printf("chown: invalid group: '%s'\n", argv[3]);
					exit(1);
				}
				// valid user
				pwd = getpwnam(argv[4]);
				if (!pwd)
				{
					printf("chown: invalid user: '%s'\n", argv[4]);
					exit(1);
				}

				if (chown(argv[2], pwd->pw_uid, grp->gr_gid) != 0)
				{
					printf("something goes wrong with 'chown'\n");
					exit(1);
				}
			}
			else
			{
				printf("chown: missing operand\n");
				printf("chown: -path -group -user\n");
				exit(1);
			}
		}
		// chmod 	->
		else if (!strcmp(command, "chmod"))
		{
			if (argc == 4)
			{
				struct stat st;

				if (stat(argv[2], &st) != 0)
				{
					printf("chmod: invalid file: '%s'\n", argv[2]);
					exit(1);
				}
				
			}
			else
			{
				printf("chmod: missing operand\n");
				printf("chown: -path -permissions\n");
				exit(1);
			}
		}
		// command not found
	}
	return 0;
}
int valid_time_interval(const char* nptr)
{
	if (nptr != NULL)
	{
		int i = 0;
		int points = 0;
		while (nptr[i] != '\0')
		{
			// is a non valid numeric character
			if (nptr[i] == '.') points++;
			else if (nptr[i] < '0' || nptr[i] > '9') return 2;
			if (points > 1) return 2;
			i++;
		}
		return 0; // valid number
	}
	return 1; // null ptr provided
}