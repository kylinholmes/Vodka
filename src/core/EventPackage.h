#ifndef _ENENTPACKAGE_H_
#define _ENENTPACKAGE_H_

#include <cstdint>
#include <sys/types.h>
#include <sys/uio.h>

const int32_t BUFFER_SIZE = 8192;
const int32_t EVENT_PACKAGE_POOL_MAX_SIZE = 1024;
const int32_t MAX_IOVE_COUNT = 5;

const int32_t RESPONSER_BUFFER_HEAD_LENGTH = 4096;
const int32_t RESPONSER_BUFFER_BODY_LENGTH = 65536;

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
  int32_t m_fd;
  int32_t m_eventType;
  int32_t m_res;
  char m_buffer[BUFFER_SIZE];
  int32_t len;
  char head[RESPONSER_BUFFER_HEAD_LENGTH];
  int32_t head_len;
  char body[RESPONSER_BUFFER_BODY_LENGTH];
  int32_t body_len;
  iovec ioves[MAX_IOVE_COUNT];
  int32_t iovec_cnt;
  void *user_data;
};

template<typename T,int max_len>
class LinearObjectPool{
public:
    int Init(){
        m_nowSize=0;
        for(int i = 0;i<max_len;i++){
            FreeObject(new T());
        }
        return 0;
    }

    void End(){
        for(int i = 0;i<m_nowSize;i++){
            delete m_pool[i];
        }
    }

    T* GetObject(){
        if(m_nowSize <= 0)return nullptr;
        m_nowSize--;
        return m_pool[m_nowSize];
    }

    int FreeObject(T* obj){
        if(m_nowSize>=max_len)return -1;
        m_pool[m_nowSize] = obj;
        m_nowSize++;
        return 0;
    }
private:
    T *m_pool[max_len+8];
    int m_nowSize;
};

using EventPackagePool = LinearObjectPool<EventPackage, EVENT_PACKAGE_POOL_MAX_SIZE>;

#endif