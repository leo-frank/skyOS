#ifndef _USER_SYSCALL_H_
#define _USER_SYSCALL_H_

#define SYSCALL_WRITE 64
int write(int fd, void* buffer, int len);

#endif