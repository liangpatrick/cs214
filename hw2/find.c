#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include "getdir.h"

int contains(char* string, char* patt){ //modified grep

    if(string == NULL || patt == NULL){
        return false;
    }
    int place = 0;
    int fPlace = 0;
    while (string[place] && patt[fPlace])
    { // checks if current char is lower or upper
        char temp1, temp2;

        temp1 = string[place++];
        temp2 = patt[fPlace];
        if (temp1 == temp2)
        {
            fPlace++;
        }
        if (fPlace == strlen(patt))
        {
            return true;
        }
        if (temp1 != temp2)
            {
                fPlace = 0;
            }
    }

    return false;
   
}

void recursiveFind(char* dirStr, char* patt){   //patt is pattern being searched for

    struct stat buffer;

    char** curDirFiles = getSortedFiles(dirStr);
    int i = 0;
    while(curDirFiles[i] != NULL){
        char recurDir[512];
        strcpy(recurDir, dirStr);
        strcat(recurDir, "/");
        strcat(recurDir, curDirFiles[i]);       //gets locations relative to starting point
        stat(recurDir, &buffer);

        if(contains(curDirFiles[i], patt)){
            printf("%s\n", recurDir);
        }
       
        if(S_ISDIR(buffer.st_mode) == 1){      //recursively searches if directory
            recursiveFind(recurDir, patt);
        }
        free(curDirFiles[i]);
        i++;
    }

    free(curDirFiles);
}

int main(int argc, char *argv[])
{
    if (argc != 2)      //checks if valid input
    {
        printf("Error: Invalid Param(s): %s\n", argv[1]);
        return 1;
    }

    char patt[25];
    strcpy(patt, argv[1]);
    recursiveFind(".",patt);
    
}  