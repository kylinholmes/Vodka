#include "Common.h"

#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <liburing.h>
#include <errno.h>
#include <sys/socket.h>
#include <assert.h>
#include <log.h>

int createServerSocket(int port){
    int sock;
    struct sockaddr_in srv_addr;

    sock = socket(PF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0);
    if (sock == -1){
        Error("create socket failed: {}\n", strerror(errno));
        exit(1);
    }

    int enable = 1;
    if (setsockopt(sock,
                   SOL_SOCKET, SO_REUSEADDR,
                   &enable, sizeof(int)) < 0){
        Error("set sock opt failed: {}\n", strerror(errno));
        exit(1);
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (const struct sockaddr *)&srv_addr,
        sizeof(srv_addr)) < 0){
        Error("bind socket failed: {}\n", strerror(errno));
        exit(1);
    }

    if (listen(sock, 10) < 0){
        Error("listen socket failed: {}\n", strerror(errno));
        exit(1);
    }
    return sock;
}

const int CONTROL_LEN = CMSG_LEN( sizeof(int) );
size_t send_fd(int fd,int fd_to_send){
    struct iovec iov[1];
    struct msghdr msg;
    char buf[0];

    iov[0].iov_base=buf;
    iov[0].iov_len=1;
    msg.msg_name = NULL;
    msg.msg_namelen=0;
    msg.msg_iov=iov;
    msg.msg_iovlen=1;

    cmsghdr cm;
    cm.cmsg_len=CONTROL_LEN;
    cm.cmsg_level=SOL_SOCKET;
    cm.cmsg_type=SCM_RIGHTS;
    *(int *)CMSG_DATA( &cm ) = fd_to_send;
    msg.msg_control=&cm;
    msg.msg_controllen=CONTROL_LEN;

    return sendmsg(fd,&msg,0);
}

int recv_fd(int fd){
    struct iovec iov[1];
    struct msghdr msg;
    char buf[0];

    iov[0].iov_base=buf;
    iov[0].iov_len=1;
    msg.msg_name = NULL;
    msg.msg_namelen=0;
    msg.msg_iov=iov;
    msg.msg_iovlen=1;

    cmsghdr cm;
    msg.msg_control=&cm;
    msg.msg_controllen=CONTROL_LEN;

    recvmsg(fd,&msg,0);

    int fd_to_send = *(int *)CMSG_DATA( &cm );
    return fd_to_send;
}
