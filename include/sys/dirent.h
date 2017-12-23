#ifndef _DIRENT_H
#define _DIRENT_H

/* #define NAME_MAX 255 */

#define _FILE 0
#define _DIRECTORY 5

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
/*
typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
*/

#endif
