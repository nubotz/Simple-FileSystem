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
  int byteRead = read_t(node_num, 0, dir_node, BLOCK_SIZE);
  int fileNum = byteRead / sizeof(DIR_NODE);
  int i;
  for(i=0; i<fileNum; i++){
    printf("%s",);
  }


}
