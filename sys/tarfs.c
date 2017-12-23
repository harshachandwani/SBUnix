#include <sys/defs.h>
#include <dirent.h>
#include <sys/tarfs.h>
#include <sys/paging.h>
#include <sys/generic.h>
#include <sys/string.h>
#include <sys/kprintf.h>
#include <sys/string.h>

file_struct* file_arr[100];
int tarfs_sz;


void traverse_and_print_tarfs(file_struct* file_arr[]){
	for(int i = 0;i < tarfs_sz;i++);
	//	kprintf("filename = %s, filetype = %d \n",file_arr[i]->filename,file_arr[i]->file_type);
}

void make_tarfs_tree(char* dir_path,int typeflag,uint64_t f,uint64_t l){
	char* path = (char*)kmalloc(sizeof(char*)*strlen(dir_path));
	//kprintf("path = %p", path);	
	strcpy(path,dir_path);
	file_struct* curr_fnode = root->child[2];
	file_struct* tmp_fnode,*tmp_fnode1;
	char* tmp = strtok(path,"/");
	int i;
	while(tmp != NULL){
		tmp_fnode = curr_fnode;
		for(i = 2;i < curr_fnode->l;i++){
			if(strcmp(tmp,curr_fnode->child[i]->filename) == 0){
				curr_fnode = curr_fnode->child[i];
				break;
			}
		}

		if(i == tmp_fnode->l){
			tmp_fnode1 = create_file_node(curr_fnode,tmp,typeflag,f,0,l);
			curr_fnode->child[curr_fnode->l] = tmp_fnode1;
			curr_fnode->l += 1;
		}
		tmp = strtok(NULL,"/"); 
	}

}


void tarfs_initialize(){
	root = (file_struct*)kmalloc(sizeof(file_struct));
//	root = (file_struct*)(0xFFFFFFFF80000000+ find_page());
	root->f = 0;
	root->l = 2;
	root->curr = 0;
	root->child[0] = root;
	root->child[1] = root;
	root->file_type = _DIRECTORY;
	root->inode_num = 0;
	strcpy(root->filename,"/");
	memset(root,0,4096);
	file_struct* filenode;
	filenode = create_file_node(root,"rootfs",_DIRECTORY,0,0,2);
	root->l  = root->l + 1;
	root->child[2] = filenode;

	posix_header_ustar* start = (posix_header_ustar*)&_binary_tarfs_start;
	posix_header_ustar* end = (posix_header_ustar*)&_binary_tarfs_end;
	uint64_t sz;
	int i = 0;
	file_struct* file_tmp = (file_struct*)kmalloc(sizeof(file_struct));
	
	while(start < end){
	
		//kprintf("name %s address = %p \n", start->name,start);
		sz = 0;
		sz = octalToDecimal(stoi(start->size));
		//kprintf("sz = %d \n",sz);
		strcpy(file_tmp->filename,start->name);
		if(file_tmp->filename[0] == '\0'){
			start = start + 1;
			continue;
		}
		//file_tmp->f = (uint64_t)start;
		file_tmp->file_type = stoi(start->typeflag);
		
		if(strcmp(start->typeflag,"5") == 0)
			make_tarfs_tree(start->name,_DIRECTORY,0,2);
		else
			make_tarfs_tree(start->name,_FILE,(uint64_t)(start + 1),(uint64_t)((void *)start+512+sz));
	
		if(sz > 0){
			if(sz % BLOCK_SIZ != 0){
				sz = (sz / BLOCK_SIZ) * BLOCK_SIZ;
				sz += BLOCK_SIZ;
			}

		start = (posix_header_ustar*)((uint64_t)start + BLOCK_SIZ + sz);
		}
		else
			start = start + 1;
		
		//kprintf("filename = %s, filetype = %d, sz = %d \n",file_tmp->filename,file_tmp->file_type,sz);
		//file_arr[i] = (file_struct *)kmalloc(sizeof(file_struct));
		//file_arr[i] = file_tmp;
		i++;
		//start = (posix_header_ustar*)((uint64_t)start+512+sz);
	}
	
#if 0
	while(start < end){
		kprintf("name %s\n", start->name);
		sz = 0;
		sz = octalToDecimal(stoi(start->size));
		if(sz % 512 != 0){
			sz = (sz/512)*512;
			sz += 512;
		}
		strcpy(file_tmp->filename,start->name);
		file_tmp->f = (uint64_t)start;
		file_tmp->file_type = stoi(start->typeflag);
//		if(strcmp(start->typeflag,"5") == 0)
//			make_tarfs_tree(start->name,_DIRECTORY,0,2);
//		else
//			make_tarfs_tree(start->name,_FILE,(uint64_t)(start + 1),(uint64_t)((void *)start+512+sz));
		file_arr[i] = file_tmp;
		start = (posix_header_ustar*)((uint64_t)start+512+sz);
		i++;
	}

#endif	
	tarfs_sz = i;
	//kprintf("tarfs_sz = %d \n", i );
	//traverse_and_print_tarfs(file_arr);

}


struct file_struct* create_file_node(file_struct* par_node,char* file_name,uint64_t filetype,uint64_t f,uint64_t inode_num,uint64_t l){

	file_struct* fnode = (file_struct*)kmalloc(sizeof(file_struct));
	fnode->child[0] = fnode;
	fnode->child[1] = par_node;
	strcpy(fnode->filename,file_name);
	fnode->file_type = filetype;
	fnode->f = f;
	fnode->l = l;
	fnode->curr = f;
	fnode->inode_num = inode_num;
	return fnode;
}

