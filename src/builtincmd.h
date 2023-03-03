#ifndef _BUILTINCMD_H_
#define _BUILTINCMD_H_

void runCd(char **cmd_list);
void runExit(pid_t* pid_sus);
void runJobs(pid_t* pid_sus, char** cmd_sus);
void runFg(char **cmd_list, pid_t* pid_sus, char** cmd_sus);
#endif