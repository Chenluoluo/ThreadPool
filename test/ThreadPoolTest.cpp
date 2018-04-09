#include <iostream>
#include "../ThreadPool.h"

void print()
{
    std::cout << "test" << std::endl;
}

void printString(const std::string& str)
{
    std::cout << str << std::endl;
    usleep(100*1000);
}

int main() {
    ThreadPool threadPool;
    threadPool.setMaxQueueSize(10);
    threadPool.start(5);
    threadPool.run(print);
    for (int i = 0; i < 10; ++i) {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        threadPool.run(boost::bind(printString, std::string(buf)));
    }
    threadPool.stop();

    while(1);
    return 0;
}