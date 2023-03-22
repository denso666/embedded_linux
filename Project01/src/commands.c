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
		printf("Name\t\t%s\n", buf.sysname);
		printf("Release\t\t%s\n", buf.release);
		printf("Version\t\t%s\n", buf.version);
		printf("Proccesor\t%s\n", buf.machine);
		exit(0);
	}
	else
	{
		perror("something goes wrong with 'uname'");
		exit(1);
	}
}

void __sleep__(const int intc, const char* intv[])
{
	// verify time intervals
	int status;
	int invalid_intervals = 0;
	int total_interval = 0;
	for (int i=2; i-2<intc; i++)
	{
		status = valid_time_interval(intv[i]);
		if (status == 2)
		{
			printf("sleep: invalid time interval '%s'\n", intv[i]);
			invalid_intervals = 1;
		}
		else
		{
			total_interval += atoi(intv[i]);
		}
	}
	// invalid intervals provided, exit with failure
	if (invalid_intervals) exit(1);
	else
	{
		if (sleep(total_interval) < 0)
		{
			// error on execution
			perror("something goes wrong with 'sleep'");
			exit(1);
		}
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
			printf("cat: %s: Is a directory\n", path);
			exit(1); // error: is a directory
		}
		else
		{
			size = st.st_size;
			fd = open(path, O_RDONLY);
			buf = (char*)malloc(sizeof(char)*size);
			read(fd, (void*)buf, size);
			close(fd);
			write(0, buf, size);
			free(buf);
			exit(0);
		}
	}
	else
	{
		printf("cat: %s: No such file or directory\n", path);
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
			printf("%s", buf);
		}
		close(fd);
		free(buf);
	}
	// error on fs /proc/modules
	else
	{
		perror("something goes wrong with 'lsmod'");
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
		printf("up %ld hours, %ld minutes, %ld seconds\n", hours, minutes, seconds);
		exit(0);
	}
	else
	{
		perror("something goes wrong with 'uptime'");
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
			printf("mkdir: created directory '%s'\n", path);
			exit(0);
		}
		else perror("something goes wrong with 'mkdir'");
	}
	else printf("mkdir: cannot create directory '%s': File or directory exists\n", path);
	exit(1);
}

void __chown__(const char* path, const char* group, const char* user)
{
	struct passwd* pwd;
	struct group* grp;
	struct stat st;

	// valid file
	if (stat(path, &st) != 0)
	{
		printf("chown: invalid file: '%s'\n", path);
		exit(1);
	}
	// valid group
	grp = getgrnam(group);
	if (!grp)
	{
		printf("chown: invalid group: '%s'\n", group);
		exit(1);
	}
	// valid user
	pwd = getpwnam(user);
	if (!pwd)
	{
		printf("chown: invalid user: '%s'\n", user);
		exit(1);
	}

	if (chown(path, pwd->pw_uid, grp->gr_gid))
	{
		perror("something goes wrong with 'chown'");
		exit(1);
	}
}
