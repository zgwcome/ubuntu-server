#include <iostream>
#include <thread>

std::string watch_dir = "/ftp";

extern bool Copy();
extern void Watch();
extern bool CopyToCloud();
extern void Recursive();

int main()
{
    std::thread th1(Copy);
    std::thread th2(CopyToCloud);
    std::thread th3(Watch);
    std::thread th4(Recursive);
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    return 0;
}