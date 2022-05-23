#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "Worker.h"

void Worker::loopWorker() {
    EventPackage *event, *temp;
    while (1) {
        event = uring.waitEvent();
        switch (event->m_eventType) {
            case EVENT_TYPE_RECVMSG:
                if (event->m_res == 0 || event->m_res == -1)
                    exit(1);
                if ((temp = eventPool.getObject()) == nullptr)
                    exit(1);
                temp->m_fd = event->m_res;
                temp->m_eventType = EVENT_TYPE_READ;
                uring.addRead(temp, temp->m_fd);
                uring.addRecvSocketFd(&event_for_recvmsg, m_handle.pipefd);
                break;
            case EVENT_TYPE_READ:
                if (event->m_res == 0) {
                    close(event->m_fd);
                    eventPool.freeObject(event);
                    // fprintf(stderr, "Empty request!\n");
                    continue;
                }
                completeEvent(event);
                break;
            case EVENT_TYPE_WRITE:
                event->m_eventType = EVENT_TYPE_END;
                completeEvent(event);
                break;
            case EVENT_TYPE_WRITEV:
                event->m_eventType = EVENT_TYPE_END;
                completeEvent(event);
                break;
            default:
                exit(1);
        }
    }
}

void Worker::completeEvent(EventPackage* event) {
    processEvent(event);
    switch (event->m_eventType) {
        case EVENT_TYPE_READ:
            uring.addRead(event, event->m_fd);
            break;
        case EVENT_TYPE_WRITE:
            uring.addWrite(event, event->m_fd, event->m_buffer, event->len);
            break;
        case EVENT_TYPE_WRITEV:
            uring.addWritev(event, event->m_fd, event->ioves, event->iovec_cnt);
            break;
        case EVENT_TYPE_END:
            close(event->m_fd);
            eventPool.freeObject(event);
            break;
        default:
            break;
    }
}

Worker* worker_instance;
void worker_sigint_handler(int signo) {
    printf("Worker:^C pressed. Shutting down.\n");
    worker_instance->endWorker();
    exit(0);
}

void Worker::initWorkerAsChild(ProcessHandle handle) {
    worker_instance = this;
    signal(SIGKILL, worker_sigint_handler);
    onInitWorker();
    uring.initUring();
    m_handle = handle;
    eventPool.initPool();
    uring.addRecvSocketFd(&event_for_recvmsg, m_handle.pipefd);
}

void Worker::endWorker() {
    onEndWorker();
    uring.endUring();
    eventPool.endPool();
}