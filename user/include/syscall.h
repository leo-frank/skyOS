#ifndef _USER_SYSCALL_H_
#define _USER_SYSCALL_H_

#define SYSCALL_WRITE 64
#define SYSCALL_FORK 32
int write(int fd, void* buffer, int len);
int fork();
void execve(char* filename);
int getpid();
#endif