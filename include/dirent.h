#ifndef _DIRENT_H
#define _DIRENT_H

/* #define NAME_MAX 255 */

#define fd_max 20
#define _FILE 0
#define _DIRECTORY 5
#define RDONLY 0
#define WRONLY 1
#define RDWR 2
#define stdin 0
#define stdout 1
#define stderr 2


typedef struct file_struct file_struct;
typedef struct dirent dirent;
typedef struct dir dir;
typedef struct fd fd;

struct file_struct{
	uint64_t f;
	uint64_t l;
	uint64_t curr;
	file_struct* child[30];
	char filename[30];
	int file_type;
	uint64_t inode_num;
};

file_struct* root;

struct dirent {
	uint64_t inode_num;
	char d_name[30];
};

struct dir{
	file_struct* f_node;
	dirent dir_curr;
	int fd;
	uint64_t curr;
	char dir_buff[30];
};


struct fd{
	uint64_t inode_num;
        uint64_t permission;
	uint64_t current;
        file_struct* f_node;
};
/*
typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
*/

#endif
