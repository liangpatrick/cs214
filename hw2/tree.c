#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "getdir.h"

void recursiveTree(char* dirStr, int level){

    struct stat buffer;

    char** curDirFiles = getSortedFiles(dirStr);
    int i = 0;
    while(curDirFiles[i] != NULL){  //creates space offset based on level (depth)
        for(int i = 0; i < level; i++){
            printf("  ");
        } 
        char recurDir[512];
        strcpy(recurDir, dirStr);
        strcat(recurDir, "/");
        strcat(recurDir, curDirFiles[i]);   //makes string of current file location relative to start
        stat(recurDir, &buffer);
        printf("-%s\n",curDirFiles[i]);
       
        if(S_ISDIR(buffer.st_mode) == 1){       //checks if file is a directory
            recursiveTree(recurDir, level + 1);
        }
        free(curDirFiles[i]);
        i++;
    }

    free(curDirFiles);
}

void printRecursiveTree(char* dirStr){  //initilizes the recursive tree function at "." level 0
    int level = 0;
    printf(".\n");
    recursiveTree(dirStr,level);
}

int main(int argc, char *argv[])
{
    if(argc != 1)   //checks for valid input
    {
        printf("Error: Invalid Param(s)\n");
        return 1;
    }

    printRecursiveTree(".");
}  