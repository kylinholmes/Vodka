#ifndef _WORKPROCESS_H_
#define _WORKPROCESS_H_

#include <liburing.h>
#include <netinet/in.h>

#include "EventPackage.h"
#include "Uring.h"
#include "Common.h"

class Worker{
public:
    // void initWorkerAsMaster(int sock);

    void initWorkerAsChild(ProcessHandle handle);

    void endWorker();

    void loopWorker();

private:

    void completeEvent(EventPackage* event);

private:
    int server_sock;
    Uring uring;
    EventPackage event_for_accept,event_for_recvmsg;
    EventPackagePool eventPool;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    ProcessHandle m_handle;
};

void processEvent(EventPackage* event);

void onInitWorker();

void onEndWorker();

#endif
