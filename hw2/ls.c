#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include "getdir.h"

int main(int argc, char *argv[])
{
    int extendedInfo = 0;       //determines if -l is used

    if (argc == 2 && strcmp(argv[1], "-l") == 0)    //handles file inputs
    {
        extendedInfo = 1;
    }
    else if(argc != 1)
    {
        printf("Error: Invalid Param(s)\n");
        return 1;
    }

    const char * months[12] = 
    {"Jan", "Feb", "Mar", "Apr", "May", 
    "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    char curDir[2]  = {".\0"};      //starting dir, fixed to cwd

    char** curDirFiles = getSortedFiles(curDir);
    if(curDirFiles[0] == NULL){
        return 1;
    }
    int i = 0;

    struct stat buffer;

    while(curDirFiles[i] != NULL){  
        char fullExt[512];
        strcpy(fullExt, curDir);
        strcat(fullExt, "/");
        strcat(fullExt, curDirFiles[i]); //gets files address relative to start

        stat(curDirFiles[i], &buffer);
        
        if(extendedInfo == 1){      //determines if -l flag exists and prints accordingly
            struct tm dt = *(localtime(&(buffer.st_ctime)));

            printf( (S_ISDIR(buffer.st_mode)) ? "d" : "-"); //read write permissions
            printf( (buffer.st_mode & S_IRUSR) ? "r" : "-");
            printf( (buffer.st_mode & S_IWUSR) ? "w" : "-");
            printf( (buffer.st_mode & S_IXUSR) ? "x" : "-");
            printf( (buffer.st_mode & S_IRGRP) ? "r" : "-");
            printf( (buffer.st_mode & S_IWGRP) ? "w" : "-");
            printf( (buffer.st_mode & S_IXGRP) ? "x" : "-");
            printf( (buffer.st_mode & S_IROTH) ? "r" : "-");
            printf( (buffer.st_mode & S_IWOTH) ? "w" : "-");
            printf( (buffer.st_mode & S_IXOTH) ? "x" : "-");

            struct passwd *pw = getpwuid(buffer.st_uid);
            struct group  *gr = getgrgid(buffer.st_gid);


            printf(" %s %s%7d %s %d %02d:%02d %s\n",
            pw->pw_name, gr->gr_name, (int)buffer.st_size,
            months[dt.tm_mon], dt.tm_mday,
            dt.tm_hour , dt.tm_min, 
            curDirFiles[i]);
        }else{
            printf("%s\n",curDirFiles[i]);
        }
        free(curDirFiles[i]);
        i++;
    }
    free(curDirFiles);
}  