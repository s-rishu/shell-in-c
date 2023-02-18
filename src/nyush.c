/*References:
1.https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
2.https://en.cppreference.com/w/c/string/byte/strrchr
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    //get base dir
    char* buffer = malloc(FILENAME_MAX*sizeof(char));
    getcwd(buffer, FILENAME_MAX); //handle error for large filenames
    char* basedir;
    char* lastslash = strrchr(buffer, '/');
    basedir = (lastslash != buffer) ? lastslash + 1 : buffer;

    //printf("[nyush %s]$ ", basedir);
    //fflush(stdout);

    while(1){
        //print prompt
        printf("[nyush %s]$ ", basedir);
        fflush(stdout);

        //read the command
        char* line_buffer = NULL;
        size_t line_len = 0;
        getline(&line_buffer, &line_len, stdin);
        free(line_buffer);
        
    };
    
    free(buffer);
}
