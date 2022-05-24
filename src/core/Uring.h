#ifndef _URING_H_
#define _URING_H_

#include <liburing.h>
#include <netinet/in.h>
#include "log.h"
#include "EventPackage.h"

class Uring
{
public:
    void InitUring();

    void End();

    int AddAccept(EventPackage* event,int sock,
        struct sockaddr_in *client_addr,
        socklen_t *client_addr_len);

    int AddRead(EventPackage* event,int sock);

    int addWrite(EventPackage* event,int sock,char* buf,int len);

    int AddWritev(EventPackage* event,int sock,iovec*iovecs,int iov_cnt);

    int AddRecvSocketFd(EventPackage* event,int fd);

    EventPackage* WaitEvent();
    
private:
   struct io_uring ring;
   struct io_uring_cqe *cqe;
};




#endif
