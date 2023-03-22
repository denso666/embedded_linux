#include "../headers/commands.h"

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
		perror("ls");
		exit(1);
	}
}

void __sleep__(const int intc, const char* intv[])
{
	// verify time intervals
	int status;
	int invalid_intervals = 0;
	int total_interval = 0;
	for (int i=1; i<intc; i++)
	{
		status = valid_time_interval(intv[i]);
		if (status == 2)
		{
			fprintf(stdout, "sleep: invalid time interval '%s'\n", intv[i]);
			invalid_intervals = 1;
		}
		else total_interval += atoi(intv[i]);
	}
	// invalid intervals provided, exit with failure
	if (invalid_intervals) exit(1);
	else
	{
		if (sleep(total_interval) < 0)
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
	int fd;
	int size;
	struct stat st;
	char* buf;
	// exist file or directory?
	if (stat(path, &st) != -1)
	{
		if ((st.st_mode & S_IFMT) == S_IFDIR)
		{
			fprintf(stdout, "cat: %s: Is a directory\n", path);
			exit(1); // error: is a directory
		}
		else
		{
			size = st.st_size;
			fd = open(path, O_RDONLY);
			buf = (char*)malloc(sizeof(char)*size);
			read(fd, (void*)buf, size);
			close(fd);

			fprintf(stdout, "%s", buf);
			free(buf);
			exit(0);
		}
	}
	else
	{
		perror("cat");
		exit(1); // error: file or directory not found
	}
}

void __lsmod__(void)
{
	int fd = open("/proc/modules", O_RDONLY);
	if (fd)
	{
		char* buf = malloc(1);
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
	if (stat(path, &st))
	{
		perror("chown");
		exit(1);
	}
	// valid group
	grp = getgrnam(group);
	if (!grp)
	{
		printf("chown");
		exit(1);
	}
	// valid user
	pwd = getpwnam(user);
	if (!pwd)
	{
		printf("chown");
		exit(1);
	}

	if (chown(path, pwd->pw_uid, grp->gr_gid))
	{
		printf("chown");
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