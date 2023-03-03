#ifndef _CMDUTIL_H_
#define _CMDUTIL_H_

void handleOtherCommands(char*** cmd_list, pid_t* pid_sus, char** cmd_sus, char* full_cmd_line);
void handleBuiltinCommands(char** cmd_list, pid_t* pid_sus, char** cmd_sus);
int updateCommandList(char*** cmd_list, char* full_cmd_line);

#endif