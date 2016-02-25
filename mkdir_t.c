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

/*mkdir_t creates a new directory file with the name dname under the
current working directory of tshell. The new directory file will be created even if
this is a directory file with the name dname (i.e. the new directory file will replace
the old one under the working directory).
*/
int main(int argc, char *argv[]){
  //modify superblock
  //make new inode number
  //add new folder to parent's data block list
  //add parent directory

}
