/**
 * file :	ThreadPool.h
 * author :	Rex
 * create :	2017-02-22 23:40
 * func :   线程池，用于异步
 * history:
 */

#ifndef	__THREADPOOL_H_
#define	__THREADPOOL_H_

#include <iostream>
#include <queue>
#include <vector>
#include "bs.h"
#include "thread.h"

using namespace std;
typedef struct thread_task_t{
    void*   (*run)(void*);
    void*   arg;
}thread_task_t;

class ThreadWork:public LoopThread{
public:
    ThreadWork(pthread_mutex_t* lock=NULL, pthread_cond_t* ready=NULL, std::queue<thread_task_t>* queue=NULL);
    ~ThreadWork();
    virtual void loop();
    
    queue<thread_task_t>*       m_queue;
    pthread_mutex_t*            m_lock;
    pthread_cond_t*             m_ready;
    
    std::queue<thread_task_t>   m_queue_tasks;
    pthread_mutex_t             m_queue_lock;
    pthread_cond_t              m_queue_ready;
};

class threadpool{
public:
    static threadpool* shareInstance();
    static void destroyInstance();
    
    threadpool(int thread_num = 2);
    ~threadpool();
    
    void add(void*   (*run)(void*),void* arg);
    void destroy();
    
protected:
    static threadpool*          m_instance;
    
    std::vector<ThreadWork*>    m_threads;
    std::queue<thread_task_t>   m_queue;
    pthread_mutex_t             m_lock;
    pthread_cond_t              m_ready;
};

void async_run(void*   (*run)(void*), void* arg);
#endif
