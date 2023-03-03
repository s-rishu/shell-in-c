#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>


int isValidWord(char* word){
    int idx = 0;
    char curr = word[idx];
    while(curr != '\0'){
        if(curr==' ' || curr=='\t' || curr==62 || curr==60 || curr==124 || curr==42 || curr==33 || curr==96 || curr==39 || curr==34){
            return 0;
        }
        idx++;
        curr = word[idx];
    }
    return 1;
}

int getCommandType(char*** cmd_list){
    //identify builtin commands
    int len_cmd_list = 0;
    while(cmd_list[len_cmd_list] != NULL){\
        len_cmd_list++;
    };

    int idx = 0;
    int len_curr_cmd = 0;

    char **curr_cmd = cmd_list[idx];
    while(curr_cmd[len_curr_cmd] != NULL){\
        len_curr_cmd++;
    };

    //check for builtin commands
    //cd and fg
    if (!strcmp(curr_cmd[0],"cd") || !strcmp(curr_cmd[0],"fg")){
        if (len_curr_cmd == 2 && len_cmd_list == 1){
            return 2;
        }
        return 0;
    };

    // jobs and exit
    if (!strcmp(curr_cmd[0],"jobs") || !strcmp(curr_cmd[0],"exit")){
        if (len_curr_cmd == 1 && len_cmd_list == 1){                            
            return 2;
        }
        return 0;
        
    };

    //check for other commands 
    while(curr_cmd != NULL){
        char* word = curr_cmd[0];
        int word_idx = 0;
        //check for invalid builtin keywords
        if ((!isValidWord(word)) || !strcmp(word,"cd") || !strcmp(word,"jobs") || !strcmp(word,"fg") || !strcmp(word,"exit")){
            return 0;
        }

        //check for valid io redirection
        while(word != NULL){
            //handle input redirection
            if(!strcmp(word,"<")){
                if(idx != 0 || (word_idx == len_curr_cmd-1) || !isValidWord(curr_cmd[word_idx+1])){
                    return 0; //invalid as not occured in first command
                }
            }
            // handle output redirection
            else if(!strcmp(word,">") || !strcmp(word,">>")){
                if(idx != (len_cmd_list-1)){
                    return 0; //invalid as not occured in last command 
                }
            }
            else if(!isValidWord(word)){
                return 0;
            }
            word_idx++;
            word = curr_cmd[word_idx];
        }

        idx++;
        curr_cmd = cmd_list[idx];
    }



    return 3; //other valid commands
}
