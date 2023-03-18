#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>

const char* sperm( const mode_t mode );
const char* stype( const mode_t mode );

int main( int argc, char *argv[] ) {
    struct stat buf;

    if ( argc < 2 ) printf( "Not enough arguments\n" );
    else {
    	// Get file stat's
	    if ( stat( argv[1], &buf ) == -1 ) fprintf( stderr, "File error\n" );
	    else {
	    	lstat(argv[1], &buf);
	    	printf( "PROPERTIES\n" );
	    	printf( "Type:\t%s\n", 		stype(buf.st_mode));
	    	printf( "Size:\t%d bytes\n",buf.st_size);
	    	printf( "Owner:\t%s\n", 	getpwuid( buf.st_uid )->pw_name);
	    	printf( "C_Date:\t%s", 		ctime(&buf.st_ctime)); 
	    	printf( "Mode:\t%s\n",		sperm(buf.st_mode));
	    	printf( "Inode:\t%d\n", 	(int)buf.st_ino);
	    }
    }
    return 0;
}
// Get string formated permissions
const char* sperm( const mode_t mode ) {
    static char perm_buff[10] = {0};
    int i = 0;

    // User
    perm_buff[i++] = ((mode & S_IRUSR) == S_IRUSR) ? 'r' : '-';
    perm_buff[i++] = ((mode & S_IWUSR) == S_IWUSR) ? 'w' : '-';
    perm_buff[i++] = ((mode & S_IXUSR) == S_IXUSR) ? 'x' : '-';
    // Group
    perm_buff[i++] = ((mode & S_IRGRP) == S_IRGRP) ? 'r' : '-';
    perm_buff[i++] = ((mode & S_IWGRP) == S_IWGRP) ? 'w' : '-';
    perm_buff[i++] = ((mode & S_IXGRP) == S_IXGRP) ? 'x' : '-';
    // Others
    perm_buff[i++] = ((mode & S_IROTH) == S_IROTH) ? 'r' : '-';
    perm_buff[i++] = ((mode & S_IWOTH) == S_IWOTH) ? 'w' : '-';
    perm_buff[i++] = ((mode & S_IXOTH) == S_IXOTH) ? 'x' : '-';
    perm_buff[i] = '\0';

    return perm_buff;
}
// Get string formated file type
const char* stype( const mode_t mode ) {
	static char type_buff[17] = {0};
	char* stype;

	// S_IFMT -> Type of file
	switch (mode & S_IFMT) {
	   case S_IFBLK:  stype = "Block device";		break;
	   case S_IFCHR:  stype = "Character device";	break;
	   case S_IFDIR:  stype = "Directory";			break;
	   case S_IFIFO:  stype = "FIFO/pipe";			break;
	   case S_IFLNK:  stype = "Symlink";			break;
	   case S_IFREG:  stype = "Regular file";		break;
	   case S_IFSOCK: stype = "Socket";				break;
	   default:       stype = "Unknown";			break;
    }
    strcpy(type_buff, stype);
	return type_buff;
}