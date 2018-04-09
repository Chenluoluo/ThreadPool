//
// Created by 陈希 on 2018/4/8.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool():
    _mutex(),_notFull(_mutex),_notEmpty(_mutex),_maxQueueSize(0),_running(false)
{

}

ThreadPool::~ThreadPool() {
    if (_running)
        stop();
}

void ThreadPool::start(int numThreads) {
    assert(_threads.empty());
    _running = true;
    _threads.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        _threads.push_back(new Thread(
                boost::bind(&ThreadPool::runInThread, this)));
        _threads[i].run();
    }
    if (numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop() {
    _mutex.lock();
    _running = false;
    _notEmpty.wakeAll();
    for_each(_threads.begin(),
        _threads.end(),
        boost::bind(&Thread::join,_1));
    _mutex.unlock();
}

void ThreadPool::run(const Task &task) {
    if (_threads.empty())
    {
        task();
    }
    else
    {
        _mutex.lock();
        while (isFull())
        {
            _notFull.wait();
        }
        assert(!isFull());

        queue_.push_back(task);
        _notEmpty.wakeOne();
        _mutex.unlock();
    }
}

bool ThreadPool::isFull() {
    bool isFull = _maxQueueSize > 0 && queue_.size() >= _maxQueueSize;
    return isFull;
}

void ThreadPool::runInThread() {

    try
    {
        if (threadInitCallback_)
        {
            threadInitCallback_();
        }
        while (_running)
        {
            Task task(take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool\n");
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool\n");
        throw;
    }
}

ThreadPool::Task ThreadPool::take() {
    _mutex.lock();
    while (queue_.empty() && _running)
    {
        _notEmpty.wait();
    }
    Task task;
    if (!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        if (_maxQueueSize > 0)
        {
            _notFull.wakeOne();
        }
    }
    _mutex.unlock();
    return task;
}
