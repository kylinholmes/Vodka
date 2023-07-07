#ifndef _WORKPROCESS_H_
#define _WORKPROCESS_H_

#include <cstdint>
#include <liburing.h>
#include <netinet/in.h>

#include "EventPackage.h"
#include "Uring.h"
#include "Common.h"

class Worker{
public:
    void Init(ProcessHandle handle);

    void End();

    void Loop();

private:
    void CompleteEvent(EventPackage* event);

private:
    int32_t server_sock;
    Uring uring;
    EventPackage event_for_accept, event_for_recvmsg;
    EventPackagePool eventPool;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    ProcessHandle m_handle;
};

void ProcessEvent(EventPackage* event);

// void onInitWorker();

// void onEndWorker();

#endif
