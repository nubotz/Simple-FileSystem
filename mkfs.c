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
	int fd = open ("HD", O_RDWR, 660);


	struct superblock* sb;

	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->inode_offset = INODE_OFFSET;
	sb->data_offset = DATA_OFFSET;
	sb->max_inode = MAX_INODE;
	sb->max_data_blk = MAX_DATA_BLK;
	sb->blk_size = BLOCK_SIZE;
	sb->next_available_inode = 0;
	sb->next_available_blk = 0;

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, (void *)sb, sizeof(struct superblock));

	//make the root directory
	struct inode* i_node;
	i_node = (struct inode*)malloc(sizeof(struct inode));
	i_node->i_number = sb->next_available_inode;
	i_node->i_mtime = time(0);
	i_node->i_type = 0;
	i_node->i_size = 0;
	i_node->i_blocks = 1;
	i_node->direct_blk[0] = DATA_OFFSET + sb->next_available_blk * BLOCK_SIZE;
	i_node->file_num = 0;

	lseek(fd, INODE_OFFSET, SEEK_SET);
	write(fd, (void *)i_node, sizeof(struct inode));

	//read back the root directory information
	lseek(fd, INODE_OFFSET, SEEK_SET);
	struct inode* temp = malloc(sizeof(struct inode));

	read(fd, (void*)temp, sizeof(struct inode));
	printf("###Done creating superblock and root.\n###root inode num is %i, create time is %i, file_num is %d\n", temp->i_number, (int)temp->i_mtime, temp->file_num);



	//add some file to test
	//add hi.txt
	(temp->file_num)++;
	lseek(fd, INODE_OFFSET, SEEK_SET);
	write(fd, (void *)temp, sizeof(struct inode));
	//modify the data block
	DIR_NODE* dir_content=malloc(sizeof(DIR_NODE));
	strcpy(dir_content->dir,"hi.txt");
	dir_content->inode_number=123;

	lseek(fd, temp->direct_blk[0], SEEK_SET);
	write(fd, (void *)dir_content, sizeof(DIR_NODE));

	printf("##modified test file, root inode num is %i, create time is %i, file_num is %d\n", temp->i_number, (int)temp->i_mtime, temp->file_num);





///////////////////////// end test

	//test sfs open_t(const char *pathname, int flags)
	int mynum = open_t("/hi.txt",0);
	printf("open_t(hi.txt)=%d",mynum);



	return 0;
}
