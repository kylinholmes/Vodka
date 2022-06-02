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

#include "EventPackage.h"
#include "Worker.h"
#include "log.h"


#define EVENTLEN(event) (size_t)(event->ioves[1].iov_len + event->ioves[0].iov_len)

void Worker::Loop() {
    EventPackage *event, *temp;
    while (1) {
        event = uring.WaitEvent();
        Debug("0 Wait 0x{:X}\n", (size_t)event);
        switch (event->m_eventType) {
            case EVENT_TYPE_RECVMSG:
                if (event->m_res == 0 || event->m_res == -1)
                    exit(1);
                if ((temp = eventPool.GetObject()) == nullptr)
                    exit(1);
                temp->m_fd = event->m_res;
                temp->m_eventType = EVENT_TYPE_READ;
                uring.AddRead(temp, temp->m_fd);
                uring.AddRecvSocketFd(&event_for_recvmsg, m_handle.pipefd);
                Debug("1 AddRead 0x{:X}\n", (size_t)temp);
                break;
            case EVENT_TYPE_READ:
                if (event->m_res == 0) {
                    close(event->m_fd);
                    eventPool.FreeObject(event);
                    continue;
                }
                Debug("2 Read 0x{:X}\n", (size_t)event);
                CompleteEvent(event);
                break;
            case EVENT_TYPE_WRITE:
                DEBUG("3 Write 0x{:X} {} {}\n", (size_t)event, event->body_total_send, EVENTLEN(event));
		        event->body_total_send += event->m_res;
                if(event->body_total_send < EVENTLEN(event))
                    event->m_eventType = EVENT_TYPE_WRITE;
                else 
                    event->m_eventType = EVENT_TYPE_END;
                CompleteEvent(event);
                break;
            case EVENT_TYPE_WRITEV:
                event->body_total_send += event->m_res;
                Debug("4 WriteV 0x{:X} {} {}\n", (size_t)event, event->body_total_send ,EVENTLEN(event));
		        if(event->body_total_send < EVENTLEN(event)){
                    event->m_eventType = EVENT_TYPE_WRITE;
                } else {
                    event->m_eventType = EVENT_TYPE_END;
                }
                CompleteEvent(event);
                break;
            default:
                exit(1);
        }
    }
}

void Worker::CompleteEvent(EventPackage* event) {
    ProcessEvent(event);
    switch (event->m_eventType) {
        // case EVENT_TYPE_READ:
        //     uring.AddRead(event, event->m_fd);
        //     break;
        case EVENT_TYPE_WRITE:
            Debug("6 Write\n");
	        uring.addWrite(event, event->m_fd, (char*)event->ioves[1].iov_base + event->body_total_send, event->ioves[1].iov_len - event->body_total_send);
            break;
        case EVENT_TYPE_WRITEV:
	        Debug("7 WriteV {:X}\n", (size_t)(event));
            uring.AddWritev(event, event->m_fd, event->ioves, event->iovec_cnt);
            break;
        case EVENT_TYPE_END:
	        Debug("8 End {} , {:X}\n", event->m_res, (size_t)(event));
            event->body_total_send = 0;
	        close(event->m_fd);
            eventPool.FreeObject(event);
            break;
        default:
            break;
    }
}

Worker* worker_instance;
void worker_sigint_handler(int signo) {
    printf("Worker is Shutting down.\n");
    worker_instance->End();
    exit(0);
}

void Worker::Init(ProcessHandle handle) {
    worker_instance = this;
    signal(SIGKILL, worker_sigint_handler);
    // onInitWorker();
    uring.InitUring();
    m_handle = handle;
    eventPool.Init();
    uring.AddRecvSocketFd(&event_for_recvmsg, m_handle.pipefd);
}

void Worker::End() {
    // onEndWorker();
    uring.End();
    eventPool.End();
}
