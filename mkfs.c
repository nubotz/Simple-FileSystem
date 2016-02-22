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

#define SB_OFFSET 512 /* The offset of superblock region*/
#define INODE_OFFSET 4096 /* The offset of inode region */
#define DATA_OFFSET 10485760 /* The offset of data region */
#define MAX_INODE 100 /* The maximum number of inode */
#define MAX_DATA_BLK 25600 /* The maximum number of block */
#define BLOCK_SIZE 4096 /* The size per block */
#define MAX_NESTING_DIR 10 /* The nesting number of directory */
#define MAX_COMMAND_LENGTH 50 /* The maximum command length */

struct superblock{ /*The key information of filesystem */
	int inode_offset; /* The start offset of the inode region */
	int data_offset; /* The start offset of the data region */
	int max_inode; /* The maximum number of inodes */
	int max_data_blk; /* The maximum number of data blocks */
	int next_available_inode; /* The index of the next free inode */
	int next_available_blk; /* The index of the next free block */
	int blk_size; /* The size per block */
};

struct inode{ /* The structure of inode, each file has only one inode */
	int i_number; /* The inode number */
	time_t i_mtime; /* Creation time of inode*/
	int i_type; /* Regular file for 1, directory file for 0 */
	int i_size; /* The size of file */
	int i_blocks; /* The total numbers of data blocks */
	int direct_blk[2]; /*Two direct data block pointers */
	int indirect_blk; /*One indirect data block pointer */
	int file_num; /* The number of file in directory, it is 0 if it is file*/
};

typedef struct dir_mapping{ /* Record file information in directory file */
	char dir[10]; /* The file name in current directory */
	int inode_number; /* The corresponding inode number */
}DIR_NODE;


int main(){
	//set up the partition of HD
	int fd = open ("HD", O_RDWR, 660);
	lseek(fd, SB_OFFSET, SEEK_SET);
	
	struct superblock* sb;
	struct inode* i_node;
	
	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->inode_offset = INODE_OFFSET;
	sb->data_offset = DATA_OFFSET;
	sb->max_inode = MAX_INODE;
	sb->max_data_blk = MAX_DATA_BLK;
	sb->blk_size = BLOCK_SIZE;
	sb->next_available_inode = 0;
	sb->next_available_blk = 0;
	
	write(fd, (void *)sb, sizeof(struct superblock));
	
	//make the root directory
	struct inode* i_node;
	i_node = (struct inode*)malloc(sizeof(struct inode));
	i_node->i_number = sb->next_available_inode;
	i_node->i_mtime = time(0);
	i_node->i_type = 0;
	i_node->i_size = ;
	i_node->i_blocks = ;
	i_node->direct_blk[0] = ; i_node->direct_blk[1] = ;
	i_node->indirect_blk = ;
	i_node->file_num = 0;
	
	
	/*
	fd = open ("myfile", O_RDWR, 660);
	
	lseek(fd, 1024, SEEK_SET);

	write(fd, (void *)pxx, sizeof(struct xx));

	lseek(fd, 1024, SEEK_SET);

	read(fd, (void *)&y, sizeof(struct xx));

	printf("offset-%d; seconds(1970.1.1)-%ld\n", y.xx_offset, y.xx_time);
*/
	return 0;
}

int open_t( const char *pathname, int flags){
	
}

int read_t( int inode_number, int offset, void *buf, int count){
	
}

int write_t( int inode_number, int offset, void *buf, int count){
	
}
