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
	char* currentPath = malloc(1024);
	strcpy(currentPath,"/");
	while(1){
		char str[100];
		fgets (str, 100, stdin);

		if (str[(int)strlen(str)-1] == '\n'){
			printf("strlen(str)-1 is %d\n", (int)strlen(str)-1);
			str[(int)strlen(str)-1] = '\0';
			printf("now the string is %s\n",str);
		}
		//split the input, if any
		char* str_split[3];
		str_split[0] = strtok(str, " ");
		str_split[1] = strtok(NULL, " ");
		str_split[2] = strtok(NULL, " ");
		//printf("str_split[0]=%s, str_split[1]=%s",str_split[0],str_split[1]);

		if(strcmp(str_split[0], "ls_t") == 0){
			ls_t(currentPath);
			printf("done ls_t");
		}else if(strcmp(str_split[0], "cd_t") == 0){
			int inode_num = open_t(str_split[1], 2);
			//printf("!!!!!!!!! inode_num is%d",inode_num);
			if(inode_num == -1){
				printf("plz make sure it's correct absolute path, e.g. /mnt/abc/\n");
			}else{
				int fd = open("HD", O_RDWR, 660);
				struct inode temp = getInode(inode_num);
				//if it is dir but not a file
				if (temp.i_type == 0){
					strcpy(currentPath,str_split[1]);
				}
			}
		}else if(strcmp(str_split[0], "mkdir_t") == 0){
			//mkdir_t(currentPath, str_split[1]);
			char* absPath = calloc(1,100);
			strcat(absPath,currentPath);
			strcat(absPath,str_split[1]);
			printf("strcmp: absPath is %s",absPath);
			open_t(absPath,1);
			free(absPath);
			printf("done mkdir_t\n");
		}else if(strcmp(str_split[0], "external_cp") == 0){
			if(external_cp(str_split[1], str_split[2]) < 0){
				printf("err");
			}else{
				printf("done");
			}
		}else if(strcmp(str_split[0], "cp_t") == 0){

		}else if(strcmp(str_split[0], "cat_t") == 0){

		}else if(strcmp(str_split[0], "exit") == 0){
			return 0;
		}else{
			printf("unknown command %s",str);
		}

		printf("%s$",currentPath);
	}

}

int ls_t(char *currentPath){
  int node_num = open_t(currentPath, 2);

	struct inode temp = getInode(node_num);
	int fileNum = temp.file_num;

  //printf("node_num here is %d", node_num);
  DIR_NODE dir_node[fileNum];
	printf("fileNum is %d, temp.time is %d",fileNum,(int)temp.i_mtime);
	loadDirContent(&dir_node[0],fileNum,&temp);

  int i;
  printf("i_num\tc_time\tf_type\tsize\tname\n");
  for(i=0 ; i<fileNum ; i++){
    int inode_number = dir_node[i].inode_number;
	  struct inode each_inode = getInode(inode_number);

    printf("%d\t%d\t%d\t%d\t%s\n", inode_number, (int)each_inode.i_mtime, each_inode.i_type, each_inode.i_size, dir_node[i].dir);
  }
}

int external_cp(char* real_path, char* sfs_path){
	int fd = open (real_path, O_RDWR, 660);
}
