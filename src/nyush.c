/*References:
1.https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
2.https://en.cppreference.com/w/c/string/byte/strrchr
3.https://stackoverflow.com/questions/59770865/strtok-c-multiple-chars-as-one-delimiter
4.https://www.geeksforgeeks.org/pipe-system-call/
5.https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
6.https://stackoverflow.com/questions/11042218/c-restore-stdout-to-terminal
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "builtincmd.h"
#include "validator.h"
#include "util.h"

int updateCommandList(char*** cmd_list){
    char* line_buffer = NULL;
    //line_buffer = NULL;  //
    size_t n = 0;
    int line_len = getline(&line_buffer, &n, stdin); //check return val TODO
    line_buffer[line_len-1] = '\0'; //remove new line
    line_len--;
    if (line_len == 0){
        return 1; //empty line
    }
    //int type = getCommandType(line_buffer);

    int idx = 0;
    char* token = multi_tok(line_buffer, " | ");

    while (token != NULL){
        char** list = malloc(1000*sizeof(char*));
        char* token1 = strtok(token, " ");
        int idx1 = 0;
        while (token1 != NULL){
             list[idx1] = token1;
             printf("list %s\n", list[idx1]);
             idx1 = idx1+1;
             token1 = strtok(NULL, " ");
         };
        list[idx1] = NULL;
        printf("should be null %s\n", list[idx1]);
        cmd_list[idx] = list;
        idx = idx+1;
        token = multi_tok(NULL, " | "); 
    };
    cmd_list[idx] = NULL;
    
    int type = getCommandType(cmd_list);
    return type;
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

void handleOtherCommands(char*** cmd_list){
    int len_cmd_list = 0;
    while(cmd_list[len_cmd_list] != NULL){\
        len_cmd_list++;
    };

    int curr_cmd_idx = 0;
    while(curr_cmd_idx < len_cmd_list){
        //printf("in while 1\n");
        char** curr_cmd = cmd_list[curr_cmd_idx];
        int len_curr_cmd= 0;
        while(curr_cmd[len_curr_cmd] != NULL){\
            len_curr_cmd++;
        };
             
        int curr_word_idx = 0;
        int curr_word_idx_end = len_curr_cmd;
        int stdin_backup = dup(0);
        int stdout_backup = dup(1);
        int in_file_des = -1;
        int out_file_des = -1;
        while(curr_cmd[curr_word_idx] != NULL){
            //printf("in while %s\n", curr_cmd[curr_word_idx]);
            //handle io redirection
            if (!strcmp(curr_cmd[curr_word_idx], "<")){
                curr_word_idx_end = curr_word_idx;
                in_file_des = open(curr_cmd[curr_word_idx+1], O_RDONLY);
                dup2(in_file_des, 0);
                if (in_file_des == -1){
                    fprintf(stderr, "Error: invalid file\n");
                    return;
                };
            }
            else if(!strcmp(curr_cmd[curr_word_idx], ">")){
                if(curr_word_idx_end > curr_word_idx){
                    curr_word_idx_end = curr_word_idx;
                };
                printf("curr filename: %s\n", curr_cmd[curr_word_idx+1]);
                out_file_des = open(curr_cmd[curr_word_idx+1], O_WRONLY|O_CREAT|O_TRUNC);
                dup2(out_file_des, 1);
                if (out_file_des == -1){
                    fprintf(stderr, "Error: output redirection error\n");
                    return;
                };
            }
            else if(!strcmp(curr_cmd[curr_word_idx], ">>")){
                if(curr_word_idx_end > curr_word_idx){
                    curr_word_idx_end = curr_word_idx;
                };
                int out_file_des = open(curr_cmd[curr_word_idx+1], O_WRONLY|O_CREAT|O_APPEND);
                dup2(out_file_des, 1);
                if (out_file_des == -1){
                    fprintf(stderr, "Error: output redirection error\n");
                    return;
                };
                }
            curr_word_idx++;
           // break;
        }

        fprintf(stdout, "curr end index %d", curr_word_idx_end);
        curr_cmd[curr_word_idx_end] = NULL; // reset arg list upto before < 

        pid_t child_pid = fork();
        if (child_pid == -1){
            printf("fork error\n");
        }
        else if (child_pid == 0){
            char full_cmd[20] = "";
            getFullPath(curr_cmd[0], full_cmd);

            curr_cmd[0] = full_cmd;
            printf("running exec");
            if (execv(curr_cmd[0], curr_cmd) == -1){
                printf("exec error\n");
            }; 
                
        }
        else{
            wait(NULL); //change to not null if status needed
            //restore stdin and stdout
            printf("exec done");
            dup2(stdin_backup, 0);
            dup2(stdout_backup, 1);

            close(in_file_des);
            close(out_file_des);
        };

        curr_cmd_idx++;
    };

    
}

void handleBuiltinCommands(char** cmd_list){
    if(!strcmp(cmd_list[0], "cd")){
        runCd(cmd_list);
    }
    else if(!strcmp(cmd_list[0], "exit")){
        runExit();
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
        char*** cmd_list = malloc(1000*sizeof(char**));
        int type = updateCommandList(cmd_list);


        switch(type){
            case 0: //invalid commands
                fprintf(stderr, "Error: invalid command\n");
                break;
            case 1: //handle empty line
                printf("DEBUG: Empty Line\n");
                break;
            case 2: //handle builtin commands
                printf("DEBUG: Builtin Command\n");
                handleBuiltinCommands(cmd_list[0]);
                break;
            case 3: //handle system commands
                printf("DEBUG: Other Commands\n");
                handleOtherCommands(cmd_list);
                break;
        };
        free(buffer);
        // TODO: free cmd_list recursively (use function from lab 1 and make sure to call on exit)

    };
    
    
}

