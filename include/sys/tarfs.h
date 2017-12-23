#include <dirent.h>
#include <sys/generic.h>
#ifndef _TARFS_H
#define _TARFS_H

#define BLOCK_SIZ 512
extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

typedef struct posix_header_ustar posix_header_ustar;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};


void traverse_and_print_tarfs(file_struct* file_arr[]);
void make_tarfs_tree(char* dir_path,int typeflag,uint64_t f,uint64_t l);
void tarfs_initialize();
struct file_struct* create_file_node(file_struct* par_node,char* file_name,uint64_t filetype,uint64_t f,uint64_t inode_num,uint64_t last);
#endif
