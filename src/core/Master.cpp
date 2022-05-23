#include <cstdlib>
#include <fmt/color.h>
#include <fmt/ostream.h>
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
#include <sys/wait.h>


#include "Master.h"
#include "Common.h"
#include "log.h"

const char* LOGO = R"(    
     _    __      __       __
    | |  / /___  / /______/ /___ _
    | | / / __ \/ //_/ __  / __ `/
    | |/ / /_/ / ,< / /_/ / /_/ /
    |___/\____/_/|_|\__,_/\__,_/

    Author: Kylin 
    Version: 0.12

)";

Master *master_instance;
void master_sigint_handler(int signo) {
    printf("Master ^C pressed. Shutting down.\n");
    master_instance->endMaster();
    exit(0);
}

void Master::Run(){
    auto port = listen_port;
    // fmt::print(fmt::fg(fmt::color::dark_sea_green) ,"{}{}","\033[1H\033[2J", LOGO);
    fmt::print(fmt::fg(fmt::color::dark_sea_green) ,"{}{}","", LOGO);
    server_sock = createServerSocket(port);
    Info("🚀 Start on http://localhost:{}\n", port);
    
    uring.initUring();

    int id = 0,pipefd[2];
    for(id = 0;id<WORKER_PORCESS_COUNT;id++){
        childHandle[id].id=id;
        socketpair(PF_UNIX,SOCK_DGRAM,0,pipefd);
        if((childHandle[id].pid = fork()) == 0) {
            for(int i = 0;i<id;i++)
               close(childHandle[i].pipefd);
            close(pipefd[1]);
            childHandle[id].pipefd = pipefd[0];
            break;
        }
        close(pipefd[0]);
        childHandle[id].pipefd=pipefd[1];
    }
    
    if(id!=WORKER_PORCESS_COUNT){
        Worker worker;
        worker.initWorkerAsChild(childHandle[id]);
        worker.loopWorker();
    }
    else {
        master_instance = this;
        signal(SIGKILL, master_sigint_handler);
        uring.addAccept(&event_for_accept,server_sock,
            &client_addr,&client_addr_len);
        loopMaster();
    }
}

void Master::loopMaster(){
    EventPackage *event;
    int sockFd,sele,stat,id;
    pid_t wpid;
    while (1){
        wpid =waitpid(-1, &stat,WNOHANG);
        if(wpid!=0 && wpid!=-1){//此时有worker异常退出
            id = rebootWorker(wpid);
            if(id != -1)break;
        }
        event = uring.waitEvent();
        switch (event->m_eventType) {
        case EVENT_TYPE_ACCEPT:
            sockFd = event->m_res;
            uring.addAccept(&event_for_accept,server_sock,
                &client_addr,&client_addr_len);
            sele = rand()%WORKER_PORCESS_COUNT;
            send_fd(childHandle[sele].pipefd,sockFd);
            close(sockFd);
            //这里异步地发送会导致短时间过多的fd，导致崩溃;所以必须同步发送，并及时关闭socket
            break;
        default: break;
        }
    }
    //退化为工作进程
    Worker worker;
    worker.initWorkerAsChild(childHandle[id]);
    worker.loopWorker();
}

void Master::endMaster(){
    uring.endUring();
    for(int i = 0;i<WORKER_PORCESS_COUNT;i++){
        close(childHandle[i].pipefd);
        kill(childHandle[i].pid,SIGKILL);
    }
}

int Master::rebootWorker(pid_t pid){
    int id = -1,pipefd[2];
    for(int i = 0;i<WORKER_PORCESS_COUNT;i++){
        if(childHandle[i].pid == pid){
            id = i;
            break;
        }
    }
    if(id == -1) return -1;
    close(childHandle[id].pipefd);

    Info("Worker end pid: {} id: {}\n",pid,id);
    childHandle[id].id=id;
    socketpair(PF_UNIX,SOCK_DGRAM,0,pipefd);
    if((childHandle[id].pid = fork()) == 0) {
        close(pipefd[1]);
        for(int i = 0;i<WORKER_PORCESS_COUNT;i++)
            if(id!=i)close(childHandle[i].pipefd);
        childHandle[id].pipefd = pipefd[0];
        return id;
    }
    close(pipefd[0]);
    childHandle[id].pipefd=pipefd[1];
    return -1;
}

