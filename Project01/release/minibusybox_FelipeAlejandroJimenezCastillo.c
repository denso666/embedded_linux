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
#include <errno.h>
#include <pwd.h>
#include <grp.h>
/**
 * Convert string to integer valid interval
 * @param nptr number pointer
 * @return On success, zero is returned. On null pointer provided, one is returned. On non valid interval, two is returned
 */
int valid_time_interval(const char*);

/**
 * Print system information
 * @return null
 */
void __uname__(void);

/**
 * List directory contents
 * @param path target directory path
 * @return null
 */
void __ls__(const char*);

/**
 * Delay for a specified amount of time
 * @param intc interval counter
 * @param intv interval values
 * @return null
 */
void __sleep__(const int, const char*[]);

/**
 * Concatenate files and print on the standard output
 * @param path target file path
 * @return null
 */
void __cat__(const char*);

/**
 * Show the status of modules in the Linux Kernel
 * @return null
 */ 
void __lsmod__(void);

/**
 * Tell how long the system has been running
 * @return null
 */ 
void __uptime__(void);

/**
 * Create the Directory(ies), if they do not already exist
 * @param path target directory name 
 * @return null
 */
void __mkdir__(const char*);

/**
 * Change the owner and group of a file
 * @param path target file or directory 
 * @param group group name
 * @param user user name
 * @return null
 */
void __chown__(const char*, const char*, const char*);

/**
 * Change file mode bits
 * @param path target file or directory
 * @param permissions string with new permissions (octal values)
 * @return null
 */
void __chmod__(const char*, const char*);

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

void __uname__(void)
{
	struct utsname buf;
	if (!uname(&buf))
	{
		fprintf(stdout, "Name\t\t%s\n", buf.sysname);
		fprintf(stdout, "Release\t\t%s\n", buf.release);
		fprintf(stdout, "Version\t\t%s\n", buf.version);
		fprintf(stdout, "Proccesor\t%s\n", buf.machine);
		exit(0);
	}
	else
	{
		perror("uname");
		exit(1);
	}
}

void __ls__(const char* path)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{	
		fprintf(stdout, "%s:\n", path);
		while ((dir = readdir(d)) != NULL)
		{
			fprintf(stdout, "%s\n", dir->d_name);
		}
		closedir(d);
		fprintf(stdout, "\n");
		exit(0);
	}
	else
	{
		fprintf(stderr, "ls: cannot open directory '%s': %s\n", path, strerror(errno));
		exit(1);
	}
}

void __sleep__(const int intc, const char* intv[])
{
	int status;
	int invalid_intervals = 0;
	float total_interval = 0.0;
	for (int i=1; i<intc; i++)
	{
		// valid time intervals format
		status = valid_time_interval(intv[i]);
		if (status == 2)
		{
			fprintf(stderr, "sleep: invalid time interval '%s'\n", intv[i]);
			invalid_intervals = 1;
		}
		else total_interval += atof(intv[i]);
	}
	// invalid intervals provided, exit with failure
	if (invalid_intervals) exit(1);
	else
	{
		if (usleep(total_interval * 1000000) < 0)
		{
			// error on execution
			perror("sleep");
			exit(1);
		}
		else exit(0);
	}
}

void __cat__(const char* path)
{
	struct stat st;
	// exist?
	if (stat(path, &st) != -1)
	{
		// is a directory?
		if ((st.st_mode & S_IFMT) == S_IFDIR)
		{
			fprintf(stderr, "cat: %s: Is a directory\n", path);
			exit(1);
		}
		else
		{
			int fd = open(path, O_RDONLY);
			char*  buf = (char*)malloc(1);
			while (read(fd, buf, 1))
			{
				fprintf(stdout, "%s", buf);
			}
			close(fd);
			free(buf);
			exit(0);
		}
	}
	else
	{
		fprintf(stderr, "cat: '%s' %s\n", path, strerror(errno));
		exit(1);
	}
}

void __lsmod__(void)
{
	int fd = open("/proc/modules", O_RDONLY);
	if (fd)
	{
		char* buf = (char*)malloc(1);
		while (read(fd, buf, 1))
		{
			fprintf(stdout, "%s", buf);
		}
		close(fd);
		free(buf);
		exit(0);
	}
	else
	{
		perror("lsmod");
		exit(1);
	}
}
 
void __uptime__(void)
{
	struct sysinfo s_info;
	if (!sysinfo(&s_info))
	{
		long seconds = s_info.uptime;
		long hours = seconds/3600;
		seconds %= 3600;
		long minutes = seconds/60;
		seconds %= 60;
		fprintf(stdout, "up %ld hours, %ld minutes, %ld seconds\n", hours, minutes, seconds);
		exit(0);
	}
	else
	{
		perror("uptime");
		exit(1);
	}
}

void __mkdir__(const char* path)
{
	struct stat st;		
	if (stat(path, &st) == -1)
	{
		if (!mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
		{
			fprintf(stdout, "mkdir: created directory '%s'\n", path);
			exit(0);
		}
		else perror("mkdir");
	}
	else fprintf(stderr, "mkdir: cannot create directory '%s': File or directory exists\n", path);
	exit(1);
}

void __chown__(const char* path, const char* group, const char* user)
{
	struct passwd* pwd;
	struct group* grp;
	struct stat st;

	// valid file
	if (stat(path, &st) == -1)
	{
		perror("chown");
		exit(1);
	}
	// group
	grp = getgrnam(group);
	if (!grp)
	{
		if (errno != 0) perror("chown");
		else fprintf( stderr, "chown: the given name or gid was not found\n");
		exit(1);
	}
	// user
	pwd = getpwnam(user);
	if (!pwd)
	{
		if (errno != 0) perror("chown");
		else fprintf( stderr, "chown: the given name or uid was not found\n");
		exit(1);
	}
	if (chown(path, pwd->pw_uid, grp->gr_gid))
	{
		perror("chown");
		exit(1);
	}
	exit(0);
}

void __chmod__(const char* path, const char* permissions)
{
	struct stat st;

	if (stat(path, &st) != 0)
	{
		printf("chmod: invalid file: '%s'\n", path);
		exit(1);
	}

	exit(0);
}