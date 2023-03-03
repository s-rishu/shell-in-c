#ifndef _UTIL_H_
#define _UTIL_H_

char *multi_tok(char *input, char *delimiter);
void addSuspendedJob(pid_t pid, char* cmd, pid_t* pid_sus, char** cmd_sus);
void removeSuspendedJob(pid_t pid, pid_t* pid_sus, char** cmd_sus);

#endif