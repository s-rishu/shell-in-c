/*References:
1.https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
2.https://en.cppreference.com/w/c/string/byte/strrchr
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int isvalid(){
    //TODO
    return 1;
}

int main() {
    //get base dir
    char* buffer = malloc(FILENAME_MAX*sizeof(char));
    getcwd(buffer, FILENAME_MAX); //handle error for large filenames
    char* basedir;
    char* lastslash = strrchr(buffer, '/');
    basedir = (lastslash != buffer) ? lastslash + 1 : buffer;

    //printf("[nyush %s]$ ", basedir);
    //fflush(stdout);
    char* line_buffer = NULL;
    while(1){
        //free memory
        free(line_buffer);

        //print prompt
        printf("[nyush %s]$ ", basedir);
        fflush(stdout);

        //read the command
        line_buffer = NULL;  //
        size_t n = 0;
        int line_len = getline(&line_buffer, &n, stdin); //check return val TODO
        line_buffer[line_len-1] = '\0'; //remove new line

        //check command validity
        if (!isvalid()){
            printf("Error: invalid command\n");
            fflush(stdout);
            continue;
        };

        //handle builtin commands

        //handle simple programs
        pid_t child_pid = fork();
        if (child_pid == -1){
            printf("fork error\n");
        }
        else if (child_pid == 0){
            char *args[2];
            args[0] = line_buffer;
            args[1] = NULL; 
            printf("%s", line_buffer);

            if (execv(args[0], args) == -1){
                printf("exec error\n");
            }; 
            
        }
        else{
            wait(NULL); //change to not null if status needed
        };

    };
    
    free(buffer);
}

