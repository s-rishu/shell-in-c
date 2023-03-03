#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "builtincmd.h"
#include "validator.h"
#include "util.h"

int updateCommandList(char*** cmd_list, char* full_cmd_line){
    char* line_buffer = NULL;
    size_t n = 0;
    int line_len = getline(&line_buffer, &n, stdin); //check return val TODO
    if (line_len == -1){
        exit(EXIT_SUCCESS); //exit on eof
    }
    line_buffer[line_len-1] = '\0'; //remove new line
    line_len--;
    strcpy(full_cmd_line, line_buffer);
    if (line_len == 0){
        return 1; //empty line
    }

    int idx = 0;
    char* token = multi_tok(line_buffer, " | ");

    while (token != NULL){
        char** list = malloc(1000*sizeof(char*));
        char* token1 = strtok(token, " ");
        int idx1 = 0;
        while (token1 != NULL){
             list[idx1] = token1;
             idx1 = idx1+1;
             token1 = strtok(NULL, " ");
         };
        list[idx1] = NULL;
        cmd_list[idx] = list;
        idx = idx+1;
        token = multi_tok(NULL, " | "); 
    };
    cmd_list[idx] = NULL;
    
    int type = getCommandType(cmd_list);
    return type;
}

int getFullPath(char* cmd, char* full_cmd){

    if (cmd[0]=='/'){
        strcpy(full_cmd, cmd);
        if(access(full_cmd, F_OK)==-1){
            return 0;
        }
    }
    else{
        int idx = 0;
        int slash_flag = 0;
        while(idx < (int)strlen(cmd)){
            if (cmd[idx] == '/'){
                slash_flag = 1;
                strcpy(full_cmd, "./");
                strcat(full_cmd, cmd);
                break;
            }
            idx++;
        }
        if (!slash_flag){
            char temp[100];
            strcpy(temp, "/usr/bin/");
            strcat(temp, cmd);
            if(access(temp, F_OK)==-1){
                return 0;
            }
            strcpy(full_cmd, cmd);

        }
        else{
            if(access(full_cmd, F_OK)==-1){
                return 0;
            }
        }
        
    }
    
    return 1;
}

void handleOtherCommands(char*** cmd_list, pid_t* pid_sus, char** cmd_sus, char* full_cmd_line){
    int len_cmd_list = 0;
    while(cmd_list[len_cmd_list] != NULL){\
        len_cmd_list++;
    };

    int curr_cmd_idx = 0;
    int pipefd[2] = {-1, -1};

    int stdin_backup = dup(0);
    int stdout_backup = dup(1);

    pid_t children[20];
    int children_idx = -1;
    while(curr_cmd_idx < len_cmd_list){

        char** curr_cmd = cmd_list[curr_cmd_idx];
        int len_curr_cmd = 0;
        while(curr_cmd[len_curr_cmd] != NULL){\
            len_curr_cmd++;
        };
             
        int curr_word_idx = 0;
        int curr_word_idx_end = len_curr_cmd;
        int in_file_des = -1;
        int out_file_des = -1;
        
        while(curr_cmd[curr_word_idx] != NULL){
            //handle io redirection
            if (!strcmp(curr_cmd[curr_word_idx], "<")){
                if(curr_word_idx_end > curr_word_idx){
                    curr_word_idx_end = curr_word_idx;
                };
                in_file_des = open(curr_cmd[curr_word_idx+1], O_RDONLY);
                dup2(in_file_des, 0);               
                if (in_file_des == -1){
                    fprintf(stderr, "Error: invalid file\n");
                    return;
                };
                close(in_file_des);
            }
            else if(!strcmp(curr_cmd[curr_word_idx], ">")){
                if(curr_word_idx_end > curr_word_idx){
                    curr_word_idx_end = curr_word_idx;
                };
                out_file_des = open(curr_cmd[curr_word_idx+1], O_WRONLY|O_CREAT|O_TRUNC);
                dup2(out_file_des, 1);
                if (out_file_des == -1){
                    fprintf(stderr, "Error: output redirection error\n");
                    return;
                };
                close(out_file_des);
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
                close(out_file_des);
                }

            
            curr_word_idx++;
        }

        curr_cmd[curr_word_idx_end] = NULL; // reset arg list upto before < 

        //handle piping
        if (len_cmd_list > 1 && curr_cmd_idx < len_cmd_list-1){
            int pipe_ret = pipe(pipefd);
            if (pipe_ret == -1){
                fprintf(stderr, "Error: pipe()\n");
                return;
            };
           
            
        };

        pid_t child_pid = fork();
        if (child_pid == -1){
            fprintf(stderr, "Error: fork()\n");
        }
        else if (child_pid == 0){

            //reset signal handlers to default
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            if (len_cmd_list > 1 && curr_cmd_idx < len_cmd_list-1){
                close(pipefd[0]);
                dup2(pipefd[1], 1);
                close(pipefd[1]);
            }
            char full_cmd[20] = "";
            if (!getFullPath(curr_cmd[0], full_cmd)){
                fprintf(stderr, "Error: invalid program\n");
                exit(0);
            };

            curr_cmd[0] = full_cmd;
            if (execvp(curr_cmd[0], curr_cmd) == -1){
                fprintf(stderr,"Error: execv()\n");
            }; 
            exit(0);
                
        }
        else{
            dup2(stdin_backup, 0);
            dup2(stdout_backup, 1);
            if (len_cmd_list > 1 && curr_cmd_idx < len_cmd_list-1){
                close(pipefd[1]);
                dup2(pipefd[0], 0);
                close(pipefd[0]);
            }
            children_idx++;
            children[children_idx] = child_pid;
            if (len_cmd_list==1){
                int child_status;
                waitpid(child_pid, &child_status, WUNTRACED); //change to not null if status needed
                if(WIFSTOPPED(child_status) != 0){
                    addSuspendedJob(child_pid, full_cmd_line, pid_sus, cmd_sus);
                };
            };

        };

        curr_cmd_idx++;
    };

    if (len_cmd_list > 1){
        for (int i = 0; i <= children_idx; i++){
            int child_status; //hold child process status
            waitpid(children[i], &child_status, WUNTRACED);
        }
    }
}

void handleBuiltinCommands(char** cmd_list, pid_t* pid_sus, char** cmd_sus){
    if(!strcmp(cmd_list[0], "cd")){
        runCd(cmd_list);
    }
    else if(!strcmp(cmd_list[0], "exit")){
        runExit(pid_sus);
    }
    else if(!strcmp(cmd_list[0], "jobs")){
        runJobs(pid_sus, cmd_sus);
    }
    else if(!strcmp(cmd_list[0], "fg")){
        runFg(cmd_list, pid_sus, cmd_sus);
    }
}
