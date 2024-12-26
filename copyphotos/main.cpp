#include <iostream>
#include <thread>

extern bool Copy();
extern void Watch();
extern bool CopyToCloud();

int main()
{
    std::thread th1(Copy);
    std::thread th2(CopyToCloud);
    std::thread th3(Watch);
    th1.join();
    th2.join();
    th3.join();
    return 0;
}