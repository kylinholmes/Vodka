
#include "Uring.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const int CONTROL_LEN = CMSG_LEN( sizeof(int) );
struct MsgPackage{
    struct msghdr msg;
    struct iovec iov[1];
    char buf[0];
    cmsghdr cm;
};

EventPackage* Uring::WaitEvent(){
    int ret = io_uring_wait_cqe(&ring, &cqe);
    EventPackage *event = (EventPackage *)cqe->user_data;
    if (ret < 0) {
        // fprintf(stderr,"io_uring_wait_cqe");
        Error("io_uring_wait_cqe");
        exit(1);
    }
    if (cqe->res < 0) {
       Warn("Async Request Failed: {} for event {}\n", strerror(-cqe->res), event->m_eventType);
        
        exit(1);
    }
    event->m_res = cqe->res;
    MsgPackage *msgPkg;
    switch (event->m_eventType) {
    case EVENT_TYPE_RECVMSG:
        if(event->m_res == 0||event->m_res == -1) exit(1);
        msgPkg = (MsgPackage *)event->m_buffer;
        event->m_res=*(int *)CMSG_DATA( &msgPkg->cm );
        break;
    default: break;
    }
    io_uring_cqe_seen(&ring, cqe);
    return event;
}

int Uring::AddAccept(EventPackage* event,int sock,
        struct sockaddr_in *client_addr,
        socklen_t *client_addr_len){
    event->m_fd=sock;
    event->m_eventType = EVENT_TYPE_ACCEPT;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, sock, (struct sockaddr *) client_addr,
                         client_addr_len, 0);
    io_uring_sqe_set_data(sqe, event);
    io_uring_submit(&ring);
    return 0;
}

int Uring::AddRead(EventPackage* event,int sock){
    event->m_fd=sock;
    event->m_eventType=EVENT_TYPE_READ;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    memset(event->m_buffer,0,BUFFER_SIZE);
    io_uring_prep_read(sqe,sock,event->m_buffer,BUFFER_SIZE,0);
    io_uring_sqe_set_data(sqe, event);
    io_uring_submit(&ring);
    return 0;
}

int Uring::addWrite(EventPackage* event,int sock,char* buf,int len){
    event->m_fd = sock;
    event->m_eventType = EVENT_TYPE_WRITE;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_write(sqe, sock, event->m_buffer, len, 0);
    io_uring_sqe_set_data(sqe, event);
    io_uring_submit(&ring);
    return 0;
}

int Uring::AddWritev(EventPackage *event, int sock, iovec *iovecs, int iov_cnt){
    event->m_fd = sock;
    event->m_eventType = EVENT_TYPE_WRITEV;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, sock, iovecs, iov_cnt, 0);
    io_uring_sqe_set_data(sqe, event);
    io_uring_submit(&ring);
    return 0;
}


int Uring::AddRecvSocketFd(EventPackage* event,int fd){
    if((char*)event->m_buffer == nullptr)return -1;
    MsgPackage *msgPkg = (MsgPackage *)event->m_buffer;

    msgPkg->iov[0].iov_base=msgPkg->buf;
    msgPkg->iov[0].iov_len=1;
    msgPkg->msg.msg_name = NULL;
    msgPkg->msg.msg_namelen=0;
    msgPkg->msg.msg_iov=msgPkg->iov;
    msgPkg->msg.msg_iovlen=1;


    msgPkg->msg.msg_control=&msgPkg->cm;
    msgPkg->msg.msg_controllen=CONTROL_LEN;

    event->m_fd=fd;
    event->m_eventType = EVENT_TYPE_RECVMSG;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_recvmsg(sqe,fd,(msghdr*)event->m_buffer,0);
    io_uring_sqe_set_data(sqe, event);
    io_uring_submit(&ring);

    return 0;
}

void Uring::InitUring(){
    io_uring_queue_init(128, &ring, 0);
}

void Uring::End(){
    io_uring_queue_exit(&ring);
}
