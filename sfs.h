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

struct inode getInode(int inode_num){
	struct inode i_node = {};
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, INODE_OFFSET+inode_num*sizeof(struct inode), SEEK_SET);
	read(fd, &i_node, sizeof(struct inode));
	close(fd);
	return i_node;
}

int saveInode(struct inode* i_node){
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, INODE_OFFSET+i_node->i_number*sizeof(struct inode), SEEK_SET);
	write(fd, (void *)i_node, sizeof(struct inode));
	close(fd);
	return 0;
}

struct superblock getSB(){
	struct superblock sb = {};
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, SB_OFFSET, SEEK_SET);
	read(fd, &sb, sizeof(struct superblock));
	close(fd);
	return sb;
}

int saveSB(struct superblock* sb){
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, (void*)sb, sizeof(struct superblock));
	close(fd);
	return 1;
}

int loadDirContent(DIR_NODE* dir_node, int fileNum, struct inode* i_node){
	int fd = open("HD", O_RDWR, 660);
	lseek(fd, i_node->direct_blk[0], SEEK_SET);

	int i;
	for(i=0;i<fileNum;i++){
		read(fd, &dir_node[i], sizeof(DIR_NODE));
		printf("loadfunction: done %d\n",i);
	}
	close(fd);
	return 1;
}

//returns inode number of file or -1 if error
//flag 0=new file; 1=new dir; 2=existing file;
int open_t(const char *pathname, int flags){
	//printf("open_t: received %s\n",pathname);
	//split the pathname
	int count_layer = 0;
	char* path_name = malloc(11);
	strcpy(path_name, pathname);
	char* str[11];
	str[0] = strtok(path_name, "/");
	printf("str[%d]=%s\n",count_layer,str[count_layer]);
	count_layer++;

	while((str[count_layer] = strtok(NULL, "/"))!=NULL){
		printf("str[%d]=%s\n",count_layer,str[count_layer]);
		count_layer++;
	}

	//printf("count_layer == %d",count_layer);
	if(str[0] == NULL){
		//pathname is root
		return 0;
	}


	int fd = open("HD", O_RDWR, 660);

	//go through each layer
	int inum_desired = 0;//start from root inode_num 0
	int i, j;
	for(i=0; i<count_layer; i++){
		//scan the inodes, start from root dir
		struct inode dir_node = getInode(inum_desired);
		int dir_fileNum = dir_node.file_num;

		//load the current directory data block into ram
		DIR_NODE dir_content[dir_fileNum];
		loadDirContent(dir_content, dir_fileNum, &dir_node);

		int foundTheFile = 0;
		for(j=0; j<dir_fileNum; j++){
			//printf("j=%d, %s == %s ????\n",j,str[i],(dir_content[j]).dir);
			if (strcmp(str[i],(dir_content[j]).dir)==0){
				inum_desired = (dir_content[j]).inode_number;
				if(i==count_layer-1){
					return inum_desired;
				}
				foundTheFile = 1;
				break;
			}
		}

		if(!foundTheFile){
			if(flags == 2){
				return -1;//existing file not found
			}else{
				//create that dir / file
				//modify parent inode
				//get superblock
				struct superblock sb = getSB();

				//modify parent inode
				int parent_inode_num = dir_node.i_number;
				dir_node.file_num++;

				//make new inode
				struct inode* temp = malloc(sizeof(struct inode));
				temp->i_number = sb.next_available_inode;
				temp->i_mtime = time(0);
				if(flags == 0){//if file
					temp->i_type = 1;
					temp->file_num = 0;
					temp->i_size = 0;
				}else if(flags == 1){
					temp->i_type = 0;
					temp->i_size = sizeof(DIR_NODE)*2;
					temp->file_num = 2;//pre define . and ..
				}

				temp->i_blocks = 1;
				temp->direct_blk[0] = DATA_OFFSET + sb.next_available_blk * BLOCK_SIZE;

				//write back sb, parent inode and new inode
				sb.next_available_inode++;
				sb.next_available_blk++;
				saveSB(&sb);
				saveInode(&dir_node);
				saveInode(temp);

				//add new folder to parent's data block list
				DIR_NODE* temp_dir = malloc(sizeof(DIR_NODE));
				strcpy(temp_dir->dir,str[i]);
				temp_dir->inode_number = temp->i_number;
				lseek(fd, dir_node.direct_blk[0]+ (dir_node.file_num-1)*sizeof(DIR_NODE), SEEK_SET);
				write(fd, temp_dir, sizeof(DIR_NODE));

				if (i==count_layer-1 && flags==0 ){
					//if last layer and it is a file, do nothing
				}else{
					//add . and .. directory
					DIR_NODE dir_content={".",temp->i_number};

					lseek(fd, temp->direct_blk[0], SEEK_SET);
					write(fd, &dir_content, sizeof(DIR_NODE));

					DIR_NODE parent_dir={"..",dir_node.i_number};
					write(fd, &parent_dir, sizeof(DIR_NODE));
				}
				inum_desired = temp->i_number;
			}
		}
	}//end for i
}

//read up to count bytes from the inode number file
//starting at offset, save into buf
//return 0 if offset exceed file size; return -1 if error; return number of byte read.
int read_t(int inode_number, int offset, void *buf, int count){
	int fd = open("HD", O_RDWR, 660);
	struct inode* temp = malloc(sizeof(struct inode));
	lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET);
	read(fd, (void*)temp, sizeof(struct inode));

	if(offset >= temp->i_size){
		return 0;
	}

	int byteRead = 0;
	//read smaller side
	if(temp->i_size > count){
		byteRead = count;
	}else{
		byteRead = temp->i_size;
	}

	lseek(fd, temp->direct_blk[0]+offset, SEEK_SET);
	return read(fd, buf, byteRead);
}

//write count byte to buf starting at offset
//number of bytes written may be less than count if there is insufficient space
int write_t(int inode_number, int offset, void *buf, int count){
	int fd = open("HD", O_RDWR, 660);
	struct inode* temp = malloc(sizeof(struct inode));
	lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET);
	read(fd, (void*)temp, sizeof(struct inode));

	lseek(fd, temp->direct_blk[0]+offset, SEEK_SET);

	int byteWrite = 0;
	if(BLOCK_SIZE - offset > count){
		byteWrite = count;
	}else{
		byteWrite = (BLOCK_SIZE - offset);
	}
	return write(fd, buf, byteWrite);
}
