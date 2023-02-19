/*References:
1.https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
2.https://en.cppreference.com/w/c/string/byte/strrchr
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "builtincmd.h"

int getCommandType(){
    //TODO
    return 1;
}

void updateCommandList(char** cmd_list){
    char* line_buffer = NULL;
    //line_buffer = NULL;  //
    size_t n = 0;
    int line_len = getline(&line_buffer, &n, stdin); //check return val TODO
    line_buffer[line_len-1] = '\0'; //remove new line
    line_len--;
        
    int idx = 0;
    char* token = strtok(line_buffer, " ");
    while (token != NULL){
        cmd_list[idx] = token;
        printf("cmdlist%s\n", cmd_list[idx]);
        idx = idx+1;
        token = strtok(NULL, " "); 
    };
    cmd_list[idx] = NULL;
}

void getFullPath(char* cmd, char* full_cmd){

    //char temp[1000] = "";
    //strcpy(temp, line_buffer);
    char* first = strtok(cmd, "/");

    if(strcmp(first, "bin")){
        strcpy(full_cmd, "/bin/");
        strcat(full_cmd, cmd);
        //printf("fullcmd%s\n", full_cmd);
    }
    else{
        strcpy(full_cmd, cmd);
    }
}

void handleBinCommands(char** cmd_list){
    pid_t child_pid = fork();
    if (child_pid == -1){
        printf("fork error\n");
    }
    else if (child_pid == 0){
        char full_cmd[20] = "";
        getFullPath(cmd_list[0], full_cmd);

        cmd_list[0] = full_cmd;

        if (execv(cmd_list[0], cmd_list) == -1){
            printf("exec error\n");
        }; 
            
    }
    else{
        wait(NULL); //change to not null if status needed
    };
}

void handleBuiltinCommands(char** cmd_list){
    if(!strcmp(cmd_list[0], "cd")){
        runCd(cmd_list);
    }
}

int main() {
    while(1){
        //free memory
        //free(line_buffer);

        //get base dir
        char* buffer = malloc(FILENAME_MAX*sizeof(char));
        getcwd(buffer, FILENAME_MAX); //handle error for large filenames
        char* basedir;
        char* lastslash = strrchr(buffer, '/');
        basedir = (lastslash != buffer) ? lastslash + 1 : buffer;

        //print prompt
        printf("[nyush %s]$ ", basedir);
        fflush(stdout);

        //read the command
        char* cmd_list[1000];
        updateCommandList(cmd_list);


        switch(getCommandType()){
            case 0: //invalid commands
                printf("Error: invalid command\n");
                break;
            case 1: //handle builtin commands
                handleBuiltinCommands(cmd_list);
                break;
            case 2: //handle system commands
                handleBinCommands(cmd_list);
                break;
        };
        free(buffer);

    };
    
    
}

