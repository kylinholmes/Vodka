#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>

struct ProcessHandle{
    int id;
    pid_t pid;
    int pipefd;
};

int createServerSocket(int port);

size_t send_fd(int fd,int fd_to_send);

int recv_fd(int fd);

#endif
