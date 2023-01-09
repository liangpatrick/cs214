#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

//change group id

typedef struct Job
{
    int jid;
    int pid;
    int status; //-1 is stopped, 1 is running, 0 is terminated
    char *command;
    char *location; //NULL if found in /bin/ or /usr/bin
    char *program;
    char grounded; //& is background, 0 is foreground
    int argc;
    int signal;
    char **argv;

    int toBeRemoved; //1 means queued to remove, 0 means remain

    struct Job *nextProccess;

} Job;

volatile int numJobs = 0;
int shellID = 0;
Job *jobs = NULL;
volatile int foregroundId = 0;
sigset_t set;

void setJobSignal(int pid, int sig)
{
    fflush(stdout);
    Job *jobPtr;
    if (jobs != NULL)
    {
        jobPtr = jobs;
    }
    else
    {
        return;
    }

    if (pid == 0)
    {
        return;
    }

    while (jobPtr->pid != pid)
    {
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return;
        }
    }
    jobPtr->signal = sig;
    return;
}

void setJobStatus(int pid, int status)
{
    fflush(stdout);
    Job *jobPtr;
    if (jobs != NULL)
    {
        jobPtr = jobs;
    }
    else
    {
        return;
    }

    if (pid == 0)
    {
        return;
    }

    while (jobPtr->pid != pid)
    {
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return;
        }
    }
    jobPtr->status = status;
    return;
}

int getJobStatus(int pid)
{
    fflush(stdout);
    Job *jobPtr;
    if (jobs != NULL)
    {
        jobPtr = jobs;
    }
    else
    {
        return -2;
    }

    if (pid == 0)
    {
        return -2;
    }

    while (jobPtr->pid != pid)
    {
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return -2;
        }
    }
    return jobPtr->status;
}

int getJobSignal(int pid)
{
    fflush(stdout);
    Job *jobPtr;
    if (jobs != NULL)
    {
        jobPtr = jobs;
    }
    else
    {
        return -1;
    }

    if (pid == 0)
    {
        return -1;
    }

    while (jobPtr->pid != pid)
    {
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return -1;
        }
    }
    return jobPtr->signal;
}

void removeJob(int pid)
{

    Job *jobPtr = jobs;
    Job *jobPrevPtr = NULL;

    if (jobPtr == NULL)
    {
        return;
    }

    while (jobPtr->pid != pid)
    {
        jobPrevPtr = jobPtr;
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return;
        }
    }

    if (jobPrevPtr == NULL)
    {
        jobs = jobPtr->nextProccess;
    }
    else
    {
        jobPrevPtr->nextProccess = jobPtr->nextProccess;
    }

    if (jobPtr->command == jobPtr->program)
    {
        free(jobPtr->command);
    }
    else
    {
        free(jobPtr->command);
        free(jobPtr->program);
    }

    free(jobPtr->location);

    for (int i = 0; i < jobPtr->argc; i++)
    {
        free(jobPtr->argv[i]);
    }
    free(jobPtr->argv);
    jobPtr->argv = NULL;
    free(jobPtr);
    jobPtr = NULL;
}

void removeAllJobs()
{
    Job *jobPtr = jobs;
    if (jobPtr == NULL)
    {
        return;
    }
    else
    {
        removeJob(jobPtr->pid);
        removeAllJobs();
    }
    return;
}

void addJob(Job *newJob)
{

    if (jobs == NULL)
    {
        jobs = newJob;
    }
    else
    {
        Job *jobsRmPtr = jobs;

        while (jobsRmPtr->nextProccess != NULL)
        {
            jobsRmPtr = jobsRmPtr->nextProccess;
        }
        jobsRmPtr->nextProccess = newJob;
    }
    newJob->nextProccess = NULL;
    numJobs++;
}

void queueRemoveJob(int pid)
{
    if (jobs == NULL || pid == -1)
    {
        return;
    }
    Job *jobPtr = jobs;

    while (jobPtr->pid != pid)
    {
        if (jobPtr->nextProccess != NULL)
        {
            jobPtr = jobPtr->nextProccess;
        }
        else
        {
            return;
        }
    }

    jobPtr->toBeRemoved = 1;
}

int clearRemovalQueue()
{
    Job *jobPtr = jobs;
    Job *nextJobPtr;

    if (jobs == NULL)
    {
        return 1;
    }

    while (jobPtr != NULL)
    {
        if (jobPtr->toBeRemoved == 1)
        {

            if (jobPtr->signal != 0)
            {

                printf("\n[%d] %d terminated by signal %d\n", jobPtr->jid, jobPtr->pid, jobPtr->signal);
            }
            else if (jobPtr->grounded == '&')
            {

                printf("\n[%d] ", jobPtr->jid);
                fflush(stdout);
                printf("Done %s\n", jobPtr->command);
                fflush(stdout);
            }

            nextJobPtr = jobPtr->nextProccess;
            removeJob(jobPtr->pid);
            jobPtr = nextJobPtr;
        }
        else
        {
            nextJobPtr = jobPtr->nextProccess;
            jobPtr = nextJobPtr;
        }
    }
    return 1;
}

void exitShell()
{
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    Job *jobsExPtr = jobs;
    while (jobsExPtr != NULL)
    {

        if (jobsExPtr->status == -1)
        {
            killpg(jobsExPtr->pid, SIGCONT);
            // waitpid(jobsExPtr->pid, NULL, 0);
        }
        killpg(jobsExPtr->pid, SIGHUP);

        waitpid(jobsExPtr->pid, NULL, 0);
        Job *rmJob = jobsExPtr;
        jobsExPtr = jobsExPtr->nextProccess;
        removeJob(rmJob->pid);
    }
    return;
}

void sig_handler(int sig)
{

    switch (sig)
    {
    case (SIGCHLD):;
        int pid;
        int status;
        
        while((pid = waitpid(-1, &status, WNOHANG)) > 0){
            if (WIFSTOPPED(status))
            {
                    
            }
            if (WIFEXITED(status))
            {
                setJobSignal(pid, 0);
                queueRemoveJob(pid);
            }
            if (WIFSIGNALED(status))
            {
                setJobSignal(pid, WTERMSIG(status));
                queueRemoveJob(pid);
            }

        }
        
        signal(SIGCHLD, sig_handler);
        break;
    case SIGINT:
        if (foregroundId == -1 || foregroundId == 0 || foregroundId == shellID)
        {
            break;
        }

        //setJobSignal(foregroundId, SIGINT);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        killpg(foregroundId, SIGINT);
        signal(SIGINT, sig_handler);

        //signal(SIGCHLD,sig_stop_handler);
        break;
    case SIGTSTP:
        if(foregroundId == 0){
            break;
        }
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        setJobSignal(foregroundId, SIGTSTP);
        setJobStatus(foregroundId, -1);
        signal(SIGTSTP, sig_handler);
        killpg(foregroundId, SIGTSTP);

        //signal(SIGCHLD,sig_stop_handler);
        break;
    case SIGTERM:
        signal(SIGTERM, sig_handler);
        break;
    }
    return;
}

void foregroundWait()
{
    int fgstatus = 0;
    int fgPid;
    
    fgPid = waitpid(foregroundId, &fgstatus, WUNTRACED);

    if (fgPid > 0){

        if (WIFSTOPPED(fgstatus))
        {
            
        }
        if (WIFEXITED(fgstatus))
        {
            setJobSignal(fgPid, 0);
            queueRemoveJob(fgPid);
        }
        if (WIFSIGNALED(fgstatus))
        {
            setJobSignal(fgPid, WTERMSIG(fgstatus));
            queueRemoveJob(fgPid);
        }
    }

}


int main(int argc, char *argv[])
{
    shellID = getpid();
    char *PWD;
    char *HOME;
    jobs = NULL;

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGCHLD, sig_handler);
    signal(SIGTSTP, sig_handler);

    errno = 0;
    PWD = getcwd(NULL, 0);
    if (PWD == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    HOME = getcwd(NULL, 0);

    char input[250];

    fflush(stdout);
    char *commandInput = NULL;
    char *absoluteDir = NULL;
    while(clearRemovalQueue() && printf("> ") && fgets(input, 250, stdin) != NULL)
    {
        sigprocmask(SIG_BLOCK, &set, NULL);

        foregroundId = 0;

        int length = (int)strlen(input);
        if (length == 1)
        {
            free(commandInput);
            commandInput = NULL;
            continue;
        }
        commandInput = (char *)malloc(sizeof(char) * length); //fix leak

        strncpy(commandInput, input, length);
        commandInput[length - 1] = '\0';

        char whitespace[2] = " ";
        char *firstArg = NULL;

        /* get the first token */
        firstArg = strtok(commandInput, whitespace);
        if(firstArg == NULL){
            free(commandInput);
            commandInput = NULL;
            continue;
        }

        if (strcmp(firstArg, "jobs") == 0)
        {
            clearRemovalQueue();
            if (jobs == NULL)
            {
                free(commandInput);
                commandInput = NULL;
                continue;
            }

            Job *jobsPtr = jobs;

            while (jobsPtr != NULL)
            {
                printf("[%d] %d ", jobsPtr->jid, jobsPtr->pid);
                if (jobsPtr->status == -1)
                {
                    printf("Stopped");
                }
                else if (jobsPtr->status == 1)
                {
                    printf("Running");
                }
                else if (jobsPtr->status == 0)
                {
                    printf("Terminated");
                }
                printf(" %s ", jobsPtr->command);
                for (int i = 1; i < jobsPtr->argc; i++)
                {
                    printf("%s ", jobsPtr->argv[i]);
                }
                printf("%c\n", jobsPtr->grounded);
                jobsPtr = jobsPtr->nextProccess;
            }
        }
        else if (strcmp(firstArg, "fg") == 0)
        {
            fflush(stdout);
            char *invalidTok;
            char *token = strtok(NULL, whitespace);
            invalidTok = strtok(NULL, whitespace);

            if (token == NULL)
            {
                printf("Please specify proccess");
            }
            else if (invalidTok != NULL)
            {
                printf("Too many arguments\n");
                free(commandInput);
                commandInput = NULL;
                continue;
            }
            if (token[0] == '%')
                memmove(token, token + 1, strlen(token));

            int jid = atoi(token);

            Job *jobsFgPtr = jobs;
            while (jobsFgPtr != NULL)
            {
                fflush(stdout);
                if (jobsFgPtr->jid != jid)
                {
                    jobsFgPtr = jobsFgPtr->nextProccess;
                }
                else
                {
                    break;
                }
            }

            if (jobsFgPtr == NULL)
            {
                printf("Unable to fg job: Job does not exist or is already in the foreground\n");
            }
            else
            {
                jobsFgPtr->grounded = 0;
                jobsFgPtr->signal = SIGCONT;
                foregroundId = jobsFgPtr->pid;
                sigprocmask(SIG_BLOCK, &set, NULL);
                killpg(jobsFgPtr->pid, SIGCONT);
                jobsFgPtr->status = 1;
                jobsFgPtr->signal = 0;
                while(killpg(foregroundId,0) == -1){
                    ;
                        }
                sigprocmask(SIG_UNBLOCK, &set, NULL);
                foregroundWait();
            }
        }
        else if (strcmp(firstArg, "bg") == 0)
        {
            fflush(stdout);
            char *invalidTok;
            char *token = strtok(NULL, whitespace);
            invalidTok = strtok(NULL, whitespace);

            if (token == NULL)
            {
                printf("Please specify proccess");
            }
            else if (invalidTok != NULL)
            {
                printf("Too many arguments\n");
                free(commandInput);
                commandInput = NULL;
                continue;
            }
            if (token[0] == '%')
                memmove(token, token + 1, strlen(token));

            int jid = atoi(token);

            Job *jobsBgPtr = jobs;
            while (jobsBgPtr != NULL)
            {
                fflush(stdout);
                if (jobsBgPtr->jid != jid)
                {
                    jobsBgPtr = jobsBgPtr->nextProccess;
                }
                else
                {
                    break;
                }
            }

            if (jobsBgPtr == NULL)
            {
                printf("Unable to bg job: Job does not exist or is already in the background\n");
            }
            else
            {
                jobsBgPtr->grounded = '&';
                jobsBgPtr->signal = SIGCONT;
                foregroundId = 0;
                sigprocmask(SIG_BLOCK, &set, NULL);
                killpg(jobsBgPtr->pid, SIGCONT);
                jobsBgPtr->status = 1;
                jobsBgPtr->signal = 0;
                sigprocmask(SIG_UNBLOCK, &set, NULL);
                while(killpg(jobsBgPtr->pid,0) == -1){
                    ;
                   }
                
            }
            free(commandInput);
            commandInput = NULL;
            continue;
        }
        else if (strcmp(firstArg, "cd") == 0)
        {
            char *invalidTok;

            char *token = strtok(NULL, whitespace);
            invalidTok = strtok(NULL, whitespace);
            if (token == NULL)
            {
                free(PWD);
                PWD = (char *)malloc(sizeof(char) * (((int)strlen(HOME)) + 1));
                PWD = strcpy(PWD, HOME);
                chdir(PWD);
                free(commandInput);
                commandInput = NULL;
                continue;
            }
            else if (invalidTok != NULL)
            {
                printf("Too many arguments\n");
                free(commandInput);
                commandInput = NULL;
                continue;
            }
            else
            {

                if (chdir(token) < 0)
                {
                    printf("%s: No such directory \n", token);
                }
                free(PWD);
                PWD = getcwd(NULL, 0);
                if (PWD == NULL)
                {
                    perror("getcwd");
                    exit(EXIT_FAILURE);
                }
                free(commandInput);
                commandInput = NULL;
                continue;
            }
        }
        else if (strcmp(firstArg, "exit") == 0)
        {
            free(commandInput);
            commandInput = NULL;

            break;
        }
        else if (strcmp(firstArg, "kill") == 0)
        {
            fflush(stdout);
            char *invalidTok;
            char *token = strtok(NULL, whitespace);
            invalidTok = strtok(NULL, whitespace);

            if (token == NULL)
            {
                printf("Please specify proccess");
            }
            else if (invalidTok != NULL)
            {
                printf("Too many arguments\n");
                free(commandInput);
                commandInput = NULL;
                continue;
            }
            if (token[0] == '%')
                memmove(token, token + 1, strlen(token));

            int jid = atoi(token);
            int pid = 0;

            Job *jobskillpgPtr = jobs;
            while (jobskillpgPtr != NULL)
            {
                fflush(stdout);
                if (jobskillpgPtr->jid != jid)
                {
                    jobskillpgPtr = jobskillpgPtr->nextProccess;
                }
                else
                {
                    pid = jobskillpgPtr->pid;
                    break;
                }
            }

            if (jobskillpgPtr == NULL)
            {
                printf("Unable to kill job: Job does not exist\n");
            }
            else
            {
                
                jobskillpgPtr->signal = SIGTERM;
                sigprocmask(SIG_BLOCK, &set, NULL);
                
            if(getJobStatus(pid) == 1){
                int status = 0;
                int wpid;
                killpg(pid, SIGTERM);
                wpid = waitpid(pid, &status, WUNTRACED);
                if (wpid > 0){

                if (WIFSTOPPED(status))
                {
            
                }
                if (WIFEXITED(status))
                {
                    setJobSignal(wpid, WEXITSTATUS(status));
                    queueRemoveJob(wpid);
                }
                if (WIFSIGNALED(status))
                {
                    setJobSignal(wpid, WTERMSIG(status));
                    queueRemoveJob(wpid);
                }
                }
            }else{
                killpg(pid, SIGTERM);
            }
                
            }
            sigprocmask(SIG_UNBLOCK, &set, NULL);
        }
        else
        {

            Job *newJob = (Job *)malloc(sizeof(Job));
            newJob->signal = 0;
            newJob->jid = numJobs + 1;
            newJob->status = 1;
            newJob->command = malloc(sizeof(char) * strlen(firstArg) + 1);
            strcpy(newJob->command, firstArg);
            newJob->grounded = 0;
            for (int i = 0; i < length; i++)
            { //parses out & to determine if background program or not
                if (commandInput[i] == '&')
                {
                    if (strchr(newJob->command, '&'))
                    {
                        newJob->command[((int)strlen(newJob->command)) - 1] = '\0';
                    }
                    commandInput[i] = ' ';
                    newJob->grounded = '&';
                }
            }

            newJob->toBeRemoved = 0;
            newJob->argc = 0;
            int maxArgc = 10;
            newJob->argv = (char **)malloc(sizeof(char *) * maxArgc);

            newJob->argv[0] = malloc(sizeof(char) * strlen(firstArg) + 1);
            strcpy(newJob->argv[newJob->argc++], firstArg);
            char *token = strtok(NULL, whitespace);

            int sIndex;
            int dirFound = 0;
            for (int i = ((int)strlen(newJob->command)) - 1; i >= 0; i--)
            { //finds last slash in program,
                if (newJob->command[i] == '/')
                { //used to determine if single program name is given, or absolute location
                    sIndex = i;
                    dirFound = 1;
                    break;
                }
            }

            if (dirFound == 1)
            { //case in which program is specified with directory

                newJob->program = malloc(sizeof(char) * (((int)(strlen(newJob->command))) - sIndex + 1));
                strncpy(newJob->program, newJob->command + sIndex, ((int)strlen(newJob->command)) - sIndex + 1);

                newJob->location = malloc(sizeof(char) * ((int)(strlen(newJob->command)) + 1)); //relative location of file
                strcpy(newJob->location, newJob->command);
                newJob->location[sIndex] = '\0';

                if (chdir(newJob->location) < 0)
                { //uses chdir to check if directory is real and sets working dir to it
                    printf("%s: No such file or directory \n", firstArg);
                    free(commandInput);
                    commandInput = NULL;
                    continue;
                }

                char *fileDir = getcwd(NULL, 0); //gets absolute directory of file (not including file name)
                if (fileDir == NULL)
                {
                    perror("getcwd");
                    exit(EXIT_FAILURE);
                }

                absoluteDir = malloc(sizeof(char) * (((int)(strlen(PWD))) + ((int)(strlen(newJob->program))) + 1));
                strcpy(absoluteDir, fileDir);
                strcat(absoluteDir, newJob->program); //combines file location with file

                free(fileDir);

                if (chdir(PWD) < 0)
                { //reverts working directory back to where it was
                    printf("Fatal Error \n");
                    exit(0);
                }
            }
            else
            { //case in which only program name is given, ie: ls, checks system programs
                newJob->program = newJob->command;
                newJob->location = NULL;

                char sysCheckOne[10] = "/usr/bin/";
                char sysCheckTwo[6] = "/bin/";

                absoluteDir = (char *)malloc(sizeof(char) * (11 + ((int)(strlen(newJob->program)))));
                strcpy(absoluteDir, sysCheckOne);
                strcat(absoluteDir, newJob->program);

                if (access(absoluteDir, F_OK) != 0)
                {
                    free(absoluteDir);
                    absoluteDir = NULL;
                    absoluteDir = (char *)malloc(sizeof(char) * (7 + ((int)(strlen(newJob->program)))));
                    strcpy(absoluteDir, sysCheckTwo);
                    strcat(absoluteDir, newJob->program);
                    if (access(absoluteDir, F_OK) != 0)
                    {
                        printf("Command \"%s\" not found\n", newJob->argv[0]);
                        newJob->pid = (-1);
                        addJob(newJob);
                        removeJob((-1));
                        free(absoluteDir);
                        absoluteDir = NULL;
                        free(commandInput);
                        commandInput = NULL;
                        continue;
                    }
                }
            }
            while (token != NULL) //makes argv for program being called
            {
                if ((newJob->argc) + 1 >= maxArgc)
                {
                    maxArgc += 10;
                    newJob->argv = (char **)realloc(newJob->argv, maxArgc * sizeof(char *));
                }

                newJob->argv[newJob->argc] = malloc(sizeof(char) * (strlen(token) + 1)); //relative location of file
                strcpy(newJob->argv[newJob->argc], token);

                token = strtok(NULL, whitespace);
                newJob->argc++;
            }
            newJob->argv[newJob->argc] = NULL;
            if (access(absoluteDir, F_OK) == 0)
            {
                
                int newPid = fork();
                if (newPid == 0)
                { // Child process
                    setpgid(0, 0);
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTERM, SIG_DFL);
                    signal(SIGCHLD, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGCONT, SIG_DFL);
                    sigprocmask(SIG_UNBLOCK, &set, NULL);

                    if (execv(absoluteDir, newJob->argv) < 0)
                    {
                        perror("File is not execuatable\n");
                        newJob->pid = (-1);
                        addJob(newJob);         
                        removeJob((-1));
                        free(commandInput);
                        commandInput = NULL;
                        break;
                    }
                }
                else if (newPid >= 0)
                {

                    newJob->pid = newPid;

                    addJob(newJob);

                    if (newJob->grounded != '&')
                    {
                        foregroundId = newJob->pid;
                        while(killpg(foregroundId,0) == -1){
                        }
                        foregroundWait();
                    }
                    else
                    {
                        printf("[%d] %d\n", newJob->jid, newJob->pid);
                    }
                }else if(newPid == -1){
                    newJob->pid = (-1);
                    addJob(newJob);
                    removeJob((-1));
                    printf("Failed to Fork New Process: Fork limit may have been reached");

                }
            }
            else
            {
                newJob->pid = (-1);
                printf("Program or File at: %s could not be found\n", newJob->argv[0]); //free mallocs if failed
                addJob(newJob);
                removeJob((-1));
            }

            free(absoluteDir);
            absoluteDir = NULL;
        }
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        free(commandInput);
        commandInput = NULL;
        fflush(stdout);
        clearRemovalQueue();
    }
    free(commandInput);
    commandInput = NULL;
    free(absoluteDir);
    absoluteDir = NULL;
    free(HOME);
    free(PWD);
    exitShell();
    clearRemovalQueue();
    removeAllJobs();
    fflush(stdout);

    return (0);
}
