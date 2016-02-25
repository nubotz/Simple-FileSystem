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
	char str[100];
	char* currentPath = "/";
	char* newDir = 
	while(1){//loop user input
		scanf("%s",str);

		if(strcmp(str, "ls_t") == 0){
				execl("./ls_t", currentPath, (char *)0);
		}else if(strcmp(str, "cd_t") == 0){

		}else if(strcmp(str, "mkdir_t") == 0){
			execl("./mkdir_t", currentPath, (char *)0);
		}else if(strcmp(str, "external_cp") == 0){

		}else if(strcmp(str, "cp_t") == 0){

		}else if(strcmp(str, "cat_t") == 0){

		}else if(strcmp(str, "exit") == 0){
			//free(currentPath);
			return 0;
		}else{
			printf("unknown command %s",str);
		}
		//printf("\n%s$",currentPath);
	}


}
