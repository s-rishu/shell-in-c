#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "util.h"

void runCd(char **cmd_list){
    //printf("chdir\n");
    //error handling
    if(access(cmd_list[1], F_OK)==-1){
        fprintf(stderr, "Error: invalid directory\n");
    }
    else if(chdir(cmd_list[1]) == -1){
        printf("chdir error\n");
    };
}

void runExit(pid_t* pid_sus){
    //printf("in run exit %d\n", getpid());
    if(pid_sus[0]==-1){
        //printf("in if exit %d\n", pid_sus[0]);
        exit(0);
    }
    fprintf(stderr, "Error: there are suspended jobs\n");
}

void runJobs(pid_t* pid_sus, char** cmd_sus){
    int idx = 0;
    while(pid_sus[idx] != -1){
        printf("[%d] %s\n", idx+1, cmd_sus[idx]);
        idx++;
    }

}

void runFg(char **cmd_list, pid_t* pid_sus, char** cmd_sus){
    int index = atoi(cmd_list[1]);
    int idx = 0;
    while(pid_sus[idx] != -1){
        if (idx+1 == index){
            pid_t pid = pid_sus[idx];
            char* cmd = cmd_sus[idx];
            removeSuspendedJob(pid, pid_sus, cmd_sus);
            int status;
            kill(pid, SIGCONT);
            //wait(&status);

            waitpid(pid, &status, WUNTRACED);//|WCONTINUED);
            if(WIFSTOPPED(status) != 0){
                addSuspendedJob(pid, cmd, pid_sus, cmd_sus);
            }
           
            return;
        }
        idx++;
    }
    fprintf(stderr, "Error: invalid job\n");
}