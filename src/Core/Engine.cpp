#include <cstdlib>

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


#include "Engine.h"
#include "Common.h"
#include "log.h"

const char* LOGO = R"(    
     _    __          ____        
    | |  / /___  ____/ / /______ _
    | | / / __ \/ __  / //_/ __ `/
    | |/ / /_/ / /_/ / ,< / /_/ / 
    |___/\____/\__,_/_/|_|\__,_/  
                              
    Author: Kylin 
    Version: {}

)";
const char* debug_info = R"(
    You are in Debug Mode
    set CMAKE_BUILD_TYPE Release in CMakeLists.txt to build release version
    Don't worry, ALL Debug info will auto clean up in release version
)";
Engine *master_instance;
void master_sigint_handler(int signo) {
    printf("Engine is Shutting down.\n");
    master_instance->End();
    exit(0);
}

void Engine::Run(){
    auto port = opt.listen_port;
    
    fmt::print(fmt::fg(fmt::color::dark_sea_green) ,LOGO, VODKA_VERSION);
    Debug("{}\n", debug_info);
    server_sock = createServerSocket(port);
    
    Info("🚀 Start on http://{}:{}\n", opt.host , port);
    
    uring.InitUring();

    int id = 0,pipefd[2];
    for(id = 0;id<opt.worker_count;id++){
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
    
    if(id!=opt.worker_count){
        Worker worker;
        worker.Init(childHandle[id]);
        worker.Loop();
    }
    else {
        master_instance = this;
        signal(SIGKILL, master_sigint_handler);
        uring.AddAccept(&event_for_accept,server_sock,
            &client_addr,&client_addr_len);
        Loop();
    }
}

void Engine::Loop(){
    EventPackage *event;
    int sockFd,sele,stat,id;
    pid_t wpid;
    while (1){
        wpid =waitpid(-1, &stat,WNOHANG);
        if(wpid!=0 && wpid!=-1){//此时有worker异常退出
            id = RebootWorker(wpid);
            if(id != -1)break;
        }
        event = uring.WaitEvent();
        switch (event->m_eventType) {
        case EVENT_TYPE_ACCEPT:
            sockFd = event->m_res;
            uring.AddAccept(&event_for_accept,server_sock,
                &client_addr,&client_addr_len);
            sele = rand()%opt.worker_count;
            send_fd(childHandle[sele].pipefd,sockFd);
            close(sockFd);
            //这里异步地发送会导致短时间过多的fd，导致崩溃;所以必须同步发送，并及时关闭socket
            break;
        default: break;
        }
    }
    //退化为工作进程
    Worker worker;
    worker.Init(childHandle[id]);
    worker.Loop();
}

void Engine::End(){
    uring.End();
    for(int i = 0;i<opt.worker_count;i++){
        close(childHandle[i].pipefd);
        kill(childHandle[i].pid,SIGKILL);
    }
}

Engine& Engine::SetOption(EngineOption option){
    this->opt = option;
    this->opt.worker_count = option.worker_count > 32? 32:option.worker_count;
    return *this;
}

int Engine::RebootWorker(pid_t pid){
    int id = -1,pipefd[2];
    for(int i = 0;i<opt.worker_count;i++){
        if(childHandle[i].pid == pid){
            id = i;
            break;
        }
    }
    if(id == -1) return -1;
    close(childHandle[id].pipefd);

    Warn("Worker end pid: {} id: {}\n",pid,id);
    childHandle[id].id=id;
    socketpair(PF_UNIX,SOCK_DGRAM,0,pipefd);
    if((childHandle[id].pid = fork()) == 0) {
        close(pipefd[1]);
        for(int i = 0;i<opt.worker_count;i++)
            if(id!=i)close(childHandle[i].pipefd);
        childHandle[id].pipefd = pipefd[0];
        return id;
    }
    close(pipefd[0]);
    childHandle[id].pipefd=pipefd[1];
    return -1;
}

