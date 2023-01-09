#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid = fork();
    if(pid ==0){
        printf("Child\n");
    } else { 
        printf("Parent of child process %d\n", pid);
    }
}