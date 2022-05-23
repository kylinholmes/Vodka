#ifndef _LINEAR_OBJECT_POOL_H_
#define _LINEAR_OBJECT_POOL_H_

#include <iostream>
#include <string_view>
#include <string>


template<typename T,int max_len>
class LinearObjectPool{
public:
    int initPool(){
        m_nowSize=0;
        for(int i = 0;i<max_len;i++){
            freeObject(new T());
        }
        return 0;
    }

    void endPool(){
        for(int i = 0;i<m_nowSize;i++){
            delete m_pool[i];
        }
    }

    T* getObject(){
        if(m_nowSize <= 0)return nullptr;
        m_nowSize--;
        return m_pool[m_nowSize];
    }

    int freeObject(T* obj){
        if(m_nowSize>=max_len)return -1;
        m_pool[m_nowSize] = obj;
        m_nowSize++;
        return 0;
    }
private:
    T *m_pool[max_len+8];
    int m_nowSize;
};

#endif