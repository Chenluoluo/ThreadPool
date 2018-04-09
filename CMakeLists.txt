cmake_minimum_required(VERSION 3.9)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(ThreadPool test/ThreadPoolTest.cpp Mutex.cpp Mutex.h Condition.cpp Condition.h Thread.cpp Thread.h ThreadPool.cpp ThreadPool.h)