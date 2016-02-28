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

struct indir_addr{
	int blk_addr;//pointer to data block
};

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
	}
	close(fd);
	return 1;
}

//returns inode number of file or -1 if error
//flag 0=new file; 1=new dir; 2=existing file;
int open_t(const char *pathname, int flags){
	//split the pathname
	int count_layer = 0;
	char* path_name = malloc(11);
	strcpy(path_name, pathname);
	char* str[11];
	str[0] = strtok(path_name, "/");
	count_layer++;

	while((str[count_layer] = strtok(NULL, "/"))!=NULL){
		count_layer++;
	}

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

				//modify parent inode
				int parent_inode_num = dir_node.i_number;
				dir_node.file_num++;
				dir_node.i_size += temp->i_size;

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
	return inum_desired;
}

//read up to count bytes from the inode number file
//starting at offset, save into buf
//return 0 if offset exceed file size; return -1 if error; return number of byte read.
int read_t(int inode_number, int offset, void *buf, int count){
	struct inode temp = getInode(inode_number);
	int fd = open("HD", O_RDWR, 660);
	if(offset > temp.i_size){
		return 0;
	}
	int byteToRead = 0;
	int readByte = 0;
	if (temp.i_size - offset > count){
		byteToRead = count;
	}else{
		byteToRead = temp.i_size - offset;
	}

	if(byteToRead<= BLOCK_SIZE){
		//read from blk[0]
		lseek(fd, temp.direct_blk[0], SEEK_SET);
		read(fd, buf, byteToRead);
		readByte += byteToRead;
	}else{
		//read from blk[0]
		lseek(fd, temp.direct_blk[0], SEEK_SET);
		read(fd, buf, BLOCK_SIZE);
		readByte += BLOCK_SIZE;
		//printf("%s",(char*) buf);

		//read from blk[1]
		lseek(fd, temp.direct_blk[1], SEEK_SET);
		if(byteToRead - readByte <= BLOCK_SIZE){
			//printf("buf is %d, buf+readByte i",*buf);
			read(fd, buf+readByte, byteToRead - readByte);
			readByte += byteToRead - readByte;
			printf("%s",(char*) buf);

		}else{
			read(fd, buf+readByte, BLOCK_SIZE);
			readByte += BLOCK_SIZE;
		}
	}
	int i;
	for(i=0;readByte < byteToRead; i++){
		//load the actual data block addr
		struct indir_addr addr = {};
		lseek(fd, temp.indirect_blk+sizeof(int)*i, SEEK_SET);
		read(fd, &addr, sizeof(int));

		lseek(fd, addr.blk_addr, SEEK_SET);
		if(byteToRead - readByte <= BLOCK_SIZE){
			//if last blk
			read(fd, buf+readByte, byteToRead - readByte);
			readByte += byteToRead - readByte;
		}else{
			read(fd, buf+readByte, BLOCK_SIZE);
			readByte += BLOCK_SIZE;
		}
	}
	close(fd);
	return readByte;
}

//write count byte from buf to HD starting at offset
//number of bytes written may be less than count if there is insufficient space
int write_t(int inode_number, int offset, void *buf, int count){
	struct inode temp = getInode(inode_number);
	struct superblock sb = getSB();
	int fd = open("HD", O_RDWR, 660);
	int blockToWrite = temp.direct_blk[0]+offset;
	float blkNum = (float)count/(float)BLOCK_SIZE;
	int writtenByte = 0;


	if(blkNum <= 1.0){
		lseek(fd, blockToWrite, SEEK_SET);
		write(fd, buf, count);
		writtenByte = count;
		//sb.next_available_blk++;
	}else{
		//dir blk[0]
		lseek(fd, blockToWrite, SEEK_SET);
		write(fd, buf, BLOCK_SIZE);
		writtenByte += BLOCK_SIZE;
		//dir blk[1]
		blockToWrite = temp.direct_blk[1] = DATA_OFFSET + sb.next_available_blk*BLOCK_SIZE;
		temp.i_blocks++;
		sb.next_available_blk++;
		if(blkNum <= 2.0){
			write(fd, buf+writtenByte, count - BLOCK_SIZE);
			writtenByte += count - BLOCK_SIZE;
		}else{
			write(fd, buf+writtenByte, BLOCK_SIZE);
			writtenByte += BLOCK_SIZE;
		}
	}
	int i;
	for(i=0;writtenByte < count;i++){
		//indir blk
		if(i==0){
			temp.indirect_blk = DATA_OFFSET + sb.next_available_blk*BLOCK_SIZE;
			sb.next_available_blk++;
		}
		struct indir_addr indir = {sb.next_available_blk*BLOCK_SIZE};
		sb.next_available_blk++;

		//write data blk addr to indirect block
		lseek(fd, temp.indirect_blk+(temp.i_blocks-2)*sizeof(int), SEEK_SET);
		write(fd, &indir, sizeof(int));

		temp.i_blocks++;
		lseek(fd, indir.blk_addr, SEEK_SET);
		if(count-writtenByte <= BLOCK_SIZE){
			//last blk
			write(fd, buf+writtenByte, count - writtenByte);
			writtenByte += count - writtenByte;
			printf("last block i =%d ***writtenByte now is %d*************\n",i, writtenByte);
		}else{
			write(fd, buf+writtenByte, BLOCK_SIZE);
			writtenByte += BLOCK_SIZE;
			printf("i =%d ***writtenByte now is %d*************\n",i, writtenByte);
		}
	}
	temp.i_size = writtenByte;
	saveInode(&temp);
	saveSB(&sb);
	return writtenByte;
}
