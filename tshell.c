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
	char* currentPath = malloc(1024);
	strcpy(currentPath,"/");
	char* newDir ;
	while(1){
		fgets (str, 100, stdin);

		//split the input, if any
		char* str_split[2];
		str_split[0] = strtok(str, " ");
		str_split[1] = strtok(NULL, "\n");
		printf("str_split[0]=%s, str_split[1]=%s",str_split[0],str_split[1]);

		if(strcmp(str_split[0], "ls_t\n") == 0){
			ls_t(currentPath);
			printf("done ls_t\n");
		}else if(strcmp(str_split[0], "cd_t") == 0){
			int inode_num = open_t(str_split[1], 2);
			printf("!!!!!!!!! inode_num is%d",inode_num);
			if(inode_num == -1){
				printf("no such absolute path\n");
			}else{
				int fd = open("HD", O_RDWR, 660);
				struct inode* temp = malloc(sizeof(struct inode));
				lseek(fd, INODE_OFFSET+inode_num*sizeof(struct inode), SEEK_SET);
				read(fd, (void*)temp, sizeof(struct inode));
				//if it is dir but not a file
				if (temp->i_type == 0){
					strcpy(currentPath,str_split[1]);
				}
			}
		}else if(strcmp(str_split[0], "mkdir_t") == 0){
			mkdir_t(currentPath, str_split[1]);
			printf("done mkdir_t\n");
		}else if(strcmp(str_split[0], "external_cp\n") == 0){

		}else if(strcmp(str_split[0], "cp_t\n") == 0){

		}else if(strcmp(str_split[0], "cat_t\n") == 0){

		}else if(strcmp(str_split[0], "exit\n") == 0){
			return 0;
		}else{
			printf("unknown command %s",str);
		}
		printf("%s$",currentPath);
	}


}

int ls_t(char *currentPath){
  int node_num = open_t(currentPath, 2);
  //printf("node_num here is %d", node_num);
  DIR_NODE* dir_node = malloc(BLOCK_SIZE);

/*
  int byteRead = read_t(node_num, 0, dir_node, BLOCK_SIZE);
  printf("byteRead = %d\n",byteRead);*/

  int fd = open("HD", O_RDWR, 660);
  struct inode* temp = malloc(sizeof(struct inode));
  lseek(fd, INODE_OFFSET+node_num*sizeof(struct inode), SEEK_SET);
  read(fd, (void*)temp, sizeof(struct inode));
	//printf("temp->file_num here is %d", temp->file_num);
  lseek(fd, temp->direct_blk[0], SEEK_SET);
	read(fd, dir_node, sizeof(DIR_NODE)*temp->file_num);

  int i;
  printf("i_num\tc_time\tf_type\tsize\tname\n");
  for(i=0 ; i< temp->file_num ; i++){
    int inode_number = dir_node[i].inode_number;

  struct inode* each_inode = malloc(sizeof(struct inode));
  lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET);
  read(fd, (void*)each_inode, sizeof(struct inode));


    printf("%d\t%d\t%d\t%d\t%s\n", inode_number, (int)each_inode->i_mtime, each_inode->i_type, each_inode->i_size, dir_node[i].dir);
  }

}