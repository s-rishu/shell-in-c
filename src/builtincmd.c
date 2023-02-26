#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void runCd(char **cmd_list){
    printf("chdir\n");
    //error handling
    if(access(cmd_list[1], F_OK)==-1){
        fprintf(stderr, "Error: invalid directory\n");
    }
    else if(chdir(cmd_list[1]) == -1){
        printf("chdir error\n");
    };
}

// void runJobs(){

// }

// void runFg(){

// }

void runExit(){
    exit(EXIT_SUCCESS); //TODO : check for suspended jobs
}