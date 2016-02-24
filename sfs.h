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


//returns inode number of file or -1 if error
//flag 0=new file; 1=new dir; 2=existing file;
int open_t(const char *pathname, int flags){
  //split the pathname
  int count_slash = 0;
	char* path_name = malloc(11);
	strcpy(path_name, pathname);
  char* str[11];
	str[0] = strtok(path_name, "/");
	printf("str[%d]=%s\n",count_slash,str[count_slash]);
	count_slash++;

	while(str[count_slash-1]!=NULL){
		str[count_slash] = strtok(NULL, "/");
		printf("str[%d]=%s\n",count_slash,str[count_slash]);
		count_slash++;
	}

	//scan the inodes, start from root dir
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, INODE_OFFSET, SEEK_SET);
	struct inode* temp = malloc(sizeof(struct inode));
	read(fd, (void*)temp, sizeof(struct inode));
	lseek(fd, temp->direct_blk[0], SEEK_SET);
	int file_num_multiplier = temp->file_num;

	//load the whole data block into ram
	DIR_NODE* dir_content = (DIR_NODE*)malloc(BLOCK_SIZE);
	read(fd, dir_content, BLOCK_SIZE);
	//get the desired node number
	int i;
	for(i=0; i<file_num_multiplier; i++){
		//assume no nesting at the moment
		printf("dir is now%s\n",(dir_content[i*sizeof(DIR_NODE)]).dir);

		if (strcmp((dir_content[i*sizeof(DIR_NODE)]).dir,str[count_slash-1])==0){
			return dir_content[i].inode_number;
		}
	}

}

int read_t(int inode_number, int offset, void *buf, int count){

}

int write_t(int inode_number, int offset, void *buf, int count){

}
