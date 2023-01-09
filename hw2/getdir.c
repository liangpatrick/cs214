#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "getdir.h"

void sort(char **array, int size){      //simple sorting algorithm for alphanumeric

    char** StringArray = array;
    int currpos = 0;
    char* holder;
    int max = size;

    while(max > 1){ //sort array of numbers
    currpos = 0;
        do{
            holder = 0;
            if((strcasecmp(StringArray[currpos], StringArray[currpos+1])>0))
            {
                holder = array[currpos + 1];
                StringArray[currpos + 1] = StringArray[currpos];
                StringArray[currpos] = holder;
            }
            currpos++;
        } while((currpos + 1) < max);
        max--;
        }
}

char** getSortedFiles(char* dirStr){ //gets all file and directory names from current dir and sorts

    int c = 20;         //initial array size
    int numInputs = 0;
    char **elements;      //elements array
    fflush(NULL);
    elements = (char**) malloc(sizeof(char*) * c);

    struct dirent *dir;
    struct stat buffer;
    DIR* dirp;
    errno = 0;
    if((dirp = opendir(dirStr)) == NULL){
        switch (errno) {
            case EACCES: printf("Permission denied\n"); break;
            case ENOENT: printf("Directory does not exist\n"); break;
            case ENOTDIR: printf("'%s' is not a directory\n", dirStr); break;
        }
        elements[0] = NULL;
        return elements;
    }
    
    while((dir = readdir(dirp)) != NULL){     

        char fullExt[512];
        strcpy(fullExt, dirStr);
        strcat(fullExt, "/");
        strcat(fullExt, dir->d_name);           //gets location relative to start as a stirng
       
        errno = 0;
        if(stat(fullExt, &buffer) < 0){ //checks if valid file
       
            switch (errno) {
                case EACCES: printf("Permission denied\n"); break;
                case ENOENT: printf("Directory does not exist\n"); break;
                case ENOTDIR: printf("'%s' is not a directory\n", dirStr); break;
            }
            continue;
        }  
    
       if((strcmp(dir->d_name, ".")==0) || (strcmp(dir->d_name, "..")==0)){
            continue;
        }

        int length = strlen(dir->d_name);
        char* tempElement = (char*) malloc(sizeof(char) * length+1);
        strncpy(tempElement, dir->d_name, length);
        tempElement[length] = '\0';
        elements[numInputs] = tempElement;
        numInputs++;
        if(numInputs+1 >= c)
        {
            c += 10;
            elements = (char**) realloc(elements, c * sizeof(char*));
        }
             
        }

        elements[numInputs] = NULL;

        closedir(dirp);

        sort(elements,numInputs);
        
    return elements;
}