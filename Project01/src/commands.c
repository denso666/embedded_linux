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

int onlyOctal(const char* optr)
{
    if (optr != NULL)
    {
        int i = 0;
        while (optr[i] != '\0')
        {
            if (optr[i] < '0' || optr[i] > '7') return 2; // invalid character provided
            i++;
        }
        return 0; // valid octal number
    }
    return 1; // null provided 
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

int __cat__(const char* path)
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
                fprintf(stderr, "cat: '%s' %s\n", path, strerror(errno));
                exit(1);
            }
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
        fprintf(stderr, "chmod: %s: %s\n", path, strerror(errno));
        exit(1);
    }
    else
    {
        if (strlen(permissions) == 3 && !onlyOctal(permissions))
        {
            int p = strtol(permissions, 0, 8);
            if (chmod(path, p) != 0)
            {
                perror("chmod");
                exit(1);
            }
            exit(0);
        }
        else
        {
            fprintf(stderr, "chmod: %s: invalid string permissions\n", permissions);
            exit(1);
        }
    }
}

void __touch__(const char* path)
{
    struct stat st;
    // exist
    if (!stat(path, &st))
    {
        // is a directory
        if ((st.st_mode & S_IFMT) == S_IFDIR)
        {
            fprintf(stderr, "touch: %s: Is a directory\n", path);
            exit(1);
        }
        // update last access and modify date
        else
        {
            struct utimbuf new_time;
            time_t t = time(NULL);
            new_time.actime = t;
            new_time.modtime = t;
            
            if (utime(path, &new_time) != 0)
            {
                fprintf(stderr, "touch: %s: %s\n", path, strerror(errno));
                exit(1);
            }
            exit(0);
        }
    }
    // create file
    else
    {
        if (creat(path, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP) < 0)
        {
            perror("touch");
            exit(1);
        }
        exit(0);
    }    
}

void __kill__(const char* pid, const char* sig)
{
    fprintf(stderr, "kill: not implemented\n");
    exit(1);
}


