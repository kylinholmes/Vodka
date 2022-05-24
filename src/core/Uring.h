#ifndef _URING_H_
#define _URING_H_

#include <liburing.h>
#include <netinet/in.h>
#include "log.h"
#include "EventPackage.h"

class Uring
{
public:
    void initUring();

    void endUring();

    int addAccept(EventPackage* event,int sock,
        struct sockaddr_in *client_addr,
        socklen_t *client_addr_len);

    int addRead(EventPackage* event,int sock);

    int addWrite(EventPackage* event,int sock,char* buf,int len);

    int addWritev(EventPackage* event,int sock,iovec*iovecs,int iov_cnt);

    //int addSendSocketFd(EventPackage* event,int fd,int sock_fd);

    int addRecvSocketFd(EventPackage* event,int fd);

    EventPackage* waitEvent();
    
private:
   struct io_uring ring;
   struct io_uring_cqe *cqe;
};




#endif
