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
	printf("###making the filesystem, plz be patience...\n");

	//set up the partition of HD

	struct superblock sb = getSB();
	sb.inode_offset = INODE_OFFSET;
	sb.data_offset = DATA_OFFSET;
	sb.max_inode = MAX_INODE;
	sb.max_data_blk = MAX_DATA_BLK;
	sb.blk_size = BLOCK_SIZE;
	sb.next_available_inode = 1;
	sb.next_available_blk = 1;
	saveSB(&sb);


	//make the root directory
	struct inode* i_node;
	i_node = (struct inode*)malloc(sizeof(struct inode));
	i_node->i_number = 0;
	i_node->i_mtime = time(0);
	i_node->i_type = 0;
	i_node->i_size = sizeof(DIR_NODE)*2;// for . and ..
	i_node->i_blocks = 1;
	i_node->direct_blk[0] = DATA_OFFSET;
	i_node->file_num = 2; //pre define . and ..
	saveInode(i_node);

	//add . and ..
	DIR_NODE dir_content={};
	strcpy(dir_content.dir,".");
	dir_content.inode_number=0;

	int fd = open ("HD", O_RDWR, 660);
	lseek(fd, DATA_OFFSET, SEEK_SET);
	write(fd, &dir_content, sizeof(DIR_NODE));
	strcpy(dir_content.dir,"..");
	write(fd, &dir_content, sizeof(DIR_NODE));

	return 0;
}
