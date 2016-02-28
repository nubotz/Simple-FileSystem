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

		if (strlen(str)>2 && str[(int)strlen(str)-1] == '\n'){
			str[(int)strlen(str)-1] = '\0';
		}
		//split the input, if any
		char* str_split[3];
		str_split[0] = strtok(str, " ");
		str_split[1] = strtok(NULL, " ");
		str_split[2] = strtok(NULL, " ");

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
			char* absPath = calloc(1,100);
			strcat(absPath,currentPath);
			strcat(absPath,"/");
			strcat(absPath,str_split[1]);
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
			cp_t(str_split[1], str_split[2]);
			printf("done cp_t\n");
		}else if(strcmp(str_split[0], "cat_t") == 0){
			cat_t(str_split[1]);
			printf("done cat_t\n");
		}else if(strcmp(str_split[0], "exit") == 0){
			return 1;
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

  DIR_NODE dir_node[fileNum];
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
	/* read the source file */
	int src = open (real_path, O_RDWR, 660); // file descripter for src file
	struct stat st; // to get the file size later
	fstat(src,&st); // get the status of src file
  ssize_t fileSize = st.st_size; // get the size
	printf("ex_cp: fileSize = %d",(int)fileSize);
	char *buf=malloc(fileSize+1); // +1 for null char
	read(src, buf, fileSize); // read the file to buf

	int newInode = open_t(sfs_path, 0);
	printf("extern_cp: newInode num is %d\n",newInode);
	int writeByte = write_t(newInode,0,buf,fileSize);
	printf("done ext_cp, writByte is %d\n",writeByte);
	free(buf);
}

int cp_t(char* src, char* des){
	int src_num = open_t(src, 3);
	int des_num = open_t(des, 0);

	//read from src to buf
	struct inode src_node = getInode(src_num);
	char* buf[src_node.i_size];
	read_t(src_num,0,buf, src_node.i_size);

	//write buf to des
	struct inode des_node = getInode(des_num);
	write_t(des_num, 0, buf, src_node.i_size);

}

int cat_t(char *filePath){
	int fileNode = open_t(filePath, 2);
	printf("fileNode is %d\n",fileNode);
	struct inode temp = getInode(fileNode);

	char *buf = calloc(1, temp.i_size+1);
	int readByte = read_t(fileNode,0,buf,temp.i_size);
	printf("\n%s\n",buf);
	free(buf);
}
