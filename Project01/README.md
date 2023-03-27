## Micro set of utilities 

| Command | Minimum operation                                                                                                                                                                 | Difficulty |
|---------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------|
| sleep   | Wait for a specified time in seconds                                                                                                                                              | 1          |
| uname   | Print: kernel name, kernel release, kernel version and processor type.                                                                                                            | 2          |
| ls      | List existing files within the indicated directory. If no directory is indicated, it should list the current directory.                                                           | 2          |
| cat     | Print the content of a file.                                                                                                                                                      | 2          |
| lsmod   | Print the names of loaded kernel modules.                                                                                                                                         | 2          |
| uptime  | Prints how long the system has been running in days, hours, minutes, and seconds.                                                                                                 | 2          |
| mkdir   | Create the indicated directory, if it does not exist.                                                                                                                             | 2          |
| rm      | Deletes the specified file, if the -r argument is given deletes the specified directory and its contents.                                                                         | 3          |
| chown   | Given the path to a valid file, group, and user. Change the owner and group of the indicated file.                                                                                | 3          |
| chmod   | Given the path to a file and a triplet of values in octal. Change the read, write and execute permissions of the indicated file.                                                  | 3          |
| touch   | Updates the access and modification dates of the indicated file. If the file does not exist it creates it empty.                                                                  | 3          |
| kill    | Sends the selected signal to the process whose PID is indicated. If no signal is indicated, it sends the TERM signal to the process.                                              | 3          |
| pidof   | Looks up and prints the PIDs that correspond to the given program name.                                                                                                           | 3          |
| killall | Sends the selected signal to all processes with the given program name. If no signal is indicated, it sends the TERM signal to the processes.                                     | 4          |
| cp      | Copies the indicated source file to the indicated destination path. If the -r argument is passed, it copies the indicated directory and all its contents (files and directories). | 4          |
