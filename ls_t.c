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

/*ls_t lists the information of all files under the current working directory
in tshell. For each file, the information should include its inode number, creation
time, file type (regular or directory), and the size of the file.
*/
int main(int argc, char *argv[]){
  int node_num = open_t(argv[0], 2);
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
