#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <time.h>
#include "sfs.h"

int main(){
	printf("tshell###\n$");
	char str[100];
	char* currentPath = "/";
	char* newDir = 
	while(1){//loop user input
		scanf("%s",str);

		if(strcmp(str, "ls_t") == 0){
			ls_t(currentPath);
		}else if(strcmp(str, "cd_t") == 0){

		}else if(strcmp(str, "mkdir_t") == 0){
			//modify superblock
			int fd = open("HD", O_RDWR, 660);
			struct superblock* sb = malloc(sizeof(struct superblock));
			lseek(fd, SB_OFFSET, SEEK_SET);
			read(fd, (void*)sb, sizeof(struct superblock));
			sb->next_available_inode;
			sb->next_available_blk;

			//make new inode
			struct inode* temp = malloc(sizeof(struct inode));
			temp->i_number = sb->next_available_inode;
			temp->i_mtime = time(0);
			temp->i_type = 0;
			temp->i_size = 0;
			temp->i_blocks = 1;
			temp->direct_blk[0] = DATA_OFFSET + sb->next_available_blk * BLOCK_SIZE;
			temp->file_num = 2; //pre define . and ..

			//write back sb and inode
			
			//add new folder to parent's data block list
			//add parent directory
		}else if(strcmp(str, "external_cp") == 0){

		}else if(strcmp(str, "cp_t") == 0){

		}else if(strcmp(str, "cat_t") == 0){

		}else if(strcmp(str, "exit") == 0){
			//free(currentPath);
			return 0;
		}else{
			printf("unknown command %s",str);
		}
		//printf("\n%s$",currentPath);
	}


}

int ls_t(char *currentPath){
  int node_num = open_t(currentPath, 2);
  DIR_NODE* dir_node = malloc(BLOCK_SIZE);

/*
  int byteRead = read_t(node_num, 0, dir_node, BLOCK_SIZE);
  printf("byteRead = %d\n",byteRead);*/

  int fd = open("HD", O_RDWR, 660);
  struct inode* temp = malloc(sizeof(struct inode));
  lseek(fd, INODE_OFFSET+node_num*sizeof(struct inode), SEEK_SET);
  read(fd, (void*)temp, sizeof(struct inode));

  lseek(fd, temp->direct_blk[0], SEEK_SET);
	read(fd, dir_node, sizeof(DIR_NODE)*temp->file_num);

  int i;
  printf("i_num\tc_time\tf_type\tsize\tname\n");
  for(i=0 ; i< temp->file_num ; i++){
    int inode_number = dir_node[i].inode_number;
    printf("%d\t%d\t%d\t%d\t%s\n", inode_number, (int)temp->i_mtime, temp->i_type, temp->i_size, dir_node[i].dir);
  }

}