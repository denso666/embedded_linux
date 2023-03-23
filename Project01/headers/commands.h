#ifndef COMMANDS_H
#define COMMANDS_H

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

#endif