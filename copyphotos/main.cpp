#include <iostream>
#include <thread>

extern bool Copy();
extern void Watch();

int main()
{
    std::thread th1(Copy);
    std::thread th2(Watch);
    th1.join();
    th2.join();
    return 0;
}