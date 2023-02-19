#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void runCd(char **cmd_list){
    printf("chdir\n");
    if(chdir(cmd_list[1]) == -1){
        printf("chdir error\n");
    };
}

// void runJobs(){

// }

// void runFg(){

// }

// void runExit(){

// }