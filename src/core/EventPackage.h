#ifndef _ENENTPACKAGE_H_
#define _ENENTPACKAGE_H_

#include "LinearObjectPool.h"

#include <sys/uio.h>

const int BUFFER_SIZE = 8192;
const int EVENT_PACKAGE_POOL_MAX_SIZE = 1000;
const int MAX_IOVE_COUNT = 5;

const int RESPONSER_BUFFER_HEAD_LENGTH = 4096;
const int RESPONSER_BUFFER_BODY_LENGTH = 65536;

enum EVENT_TYPE {
  EVENT_TYPE_ACCEPT = 0,
  EVENT_TYPE_READ = 1,
  EVENT_TYPE_WRITE = 2,
  EVENT_TYPE_WRITEV = 3,
  EVENT_TYPE_END = 4,
  EVENT_TYPE_SENDMSG = 5,
  EVENT_TYPE_RECVMSG = 6,
};

struct EventPackage {
  int m_fd;
  int m_eventType;
  int m_res;
  char m_buffer[BUFFER_SIZE];
  int len;
  char head[RESPONSER_BUFFER_HEAD_LENGTH];
  int head_len;
  char body[RESPONSER_BUFFER_BODY_LENGTH];
  int body_len;
  iovec ioves[MAX_IOVE_COUNT];
  int iovec_cnt;
  void *user_data;
};

typedef LinearObjectPool<EventPackage, EVENT_PACKAGE_POOL_MAX_SIZE>
    EventPackagePool;

#endif