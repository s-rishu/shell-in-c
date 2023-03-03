#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *multi_tok(char *input, char *delimiter) {
    static char *string;
    if (input != NULL)
        string = input;

    if (string == NULL)
        return string;

    char *end = strstr(string, delimiter);
    if (end == NULL) {
        char *temp = string;
        string = NULL;
        return temp;
    }

    char *temp = string;

    *end = '\0';
    string = end + strlen(delimiter);
    return temp;
}

void addSuspendedJob(pid_t pid, char* cmd, pid_t* pid_sus, char** cmd_sus){
    int idx = 0;
    while(pid_sus[idx] != -1){
        idx++;
    }
    pid_sus[idx] = pid;
    pid_sus[idx+1] = -1;

    cmd_sus[idx] = cmd;
    cmd_sus[idx+1] = NULL;
}

void removeSuspendedJob(pid_t pid, pid_t* pid_sus, char** cmd_sus){
    int idx = 0;
    while(pid_sus[idx] != pid){
        idx++;
    }
    while(pid_sus[idx] != -1){
        pid_sus[idx] = pid_sus[idx+1];
        cmd_sus[idx] = cmd_sus[idx+1];
        idx++;
    }
}