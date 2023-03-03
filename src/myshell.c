/*References:
1.https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
2.https://en.cppreference.com/w/c/string/byte/strrchr
3.https://stackoverflow.com/questions/59770865/strtok-c-multiple-chars-as-one-delimiter
4.https://www.geeksforgeeks.org/pipe-system-call/
5.https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
6.https://stackoverflow.com/questions/11042218/c-restore-stdout-to-terminal
7.https://man7.org/linux/man-pages/man2/pipe.2.html
8.https://stackoverflow.com/questions/36016477/pipeline-communication-between-child-process-and-parent-process-using-pipe
9.https://www.geeksforgeeks.org/signals-c-language/
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "cmdutil.h"

int main() {
    //ignore certain signals
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    //create store for suspended jobs
    pid_t* pid_sus = malloc(101*sizeof(pid_t));
    char** cmd_sus = malloc(101*sizeof(char*));
    pid_sus[0] = -1;
    while(1){
        //get base dir
        char* buffer = malloc(FILENAME_MAX*sizeof(char));
        getcwd(buffer, FILENAME_MAX); //handle error for large filenames
        char* basedir;
        char* lastslash = strrchr(buffer, '/');
        basedir = (lastslash != buffer) ? lastslash + 1 : buffer;

        //print prompt
        printf("[myshell %s]$ ", basedir);
        fflush(stdout);

        //read the command
        char* full_cmd_line = malloc(1000*sizeof(char));
        char*** cmd_list = malloc(1000*sizeof(char**));
        int type = updateCommandList(cmd_list, full_cmd_line);


        switch(type){
            case 0: //invalid commands
                fprintf(stderr, "Error: invalid command\n");
                break;
            case 1: //handle empty line
                break;
            case 2: //handle builtin commands
                handleBuiltinCommands(cmd_list[0], pid_sus, cmd_sus);
                break;
            case 3: //handle system commands
                handleOtherCommands(cmd_list, pid_sus, cmd_sus, full_cmd_line);
                break;
        };
        
        free(buffer);
        // TODO: free cmd_list recursively (use function from lab 1 and make sure to call on exit)
       
    };
    //TODO: free cmd_sus recursively
    free(pid_sus);
      
}

