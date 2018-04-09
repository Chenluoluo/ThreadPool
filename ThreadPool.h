//
// Created by 陈希 on 2018/4/8.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <deque>
#include <iostream>
#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"



class ThreadPool {
    typedef boost::function<void ()> Task;
public:
    explicit ThreadPool();
    ~ThreadPool();
    void setMaxQueueSize(int maxSize) { _maxQueueSize = (size_t)maxSize; }
    void setThreadInitCallback(const Task& cb){ threadInitCallback_ = cb; }
    void start(int numThreads);
    void stop();
    void run(const Task& task);

private:
    bool isFull();
    void runInThread();
    Task take();

    Mutex _mutex;
    Condition _notEmpty;
    Condition _notFull;
    Task threadInitCallback_;
    boost::ptr_vector<Thread> _threads;
    std::deque<Task> queue_;
    size_t _maxQueueSize;
    bool _running;
};


#endif //THREADPOOL_THREADPOOL_H
