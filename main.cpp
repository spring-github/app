/*
 * @Author: qiuzhb
 * @Date: 2023-02-28 15:23:40
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 20:51:13
 * @FilePath: /FIRE/main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */

#include <thread>
#include "common/MessageQueue.hpp"
#include "Fire.h"
#include "common/Timer.h"
#include <fstream>
#include <string.h>
#include <signal.h>

std::mutex m;
int t1Msg = 100000000;
int t2Msg = 200000000;
int t3Msg = 300000000;
int rcvMsg = 0;
int timer1Msg = 400000000;
int timer2Msg = 500000000;
int timer3Msg = 600000000;

#include <syscall.h>
pid_t gettid()
{
    return syscall(SYS_gettid);
}

void blockSignal()
{
    int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
    sigset_t blockSet;
    sigemptyset(&blockSet);

    sigaddset(&blockSet, SIGRTMIN);
    sigaddset(&blockSet, SIGRTMIN + 1);
    if (sigprocmask(SIG_SETMASK, &blockSet, NULL) == -1)
    {
        std::cout << "sigprocmask error\n";
    }
}

void thread1()
{
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread1 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    while (1)
    {
        // std::cout << "thread1 " << msg << "\n";
        fire.getMsgQueue().push(t1Msg);
        t1Msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread2()
{
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread2 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    while (1)
    {
        // std::cout << "thread2 " << msg << "\n";
        fire.getMsgQueue().push(t2Msg);
        t2Msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread3()
{
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread3 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    while (1)
    {
        // std::cout << "thread3 " << msg << "\n";
        fire.getMsgQueue().push(t3Msg);
        t3Msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread4()
{
    std::cout << "thread4 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    std::ofstream logfile("log.txt", std::ofstream::binary);
    // allocate memory for file content
    char *buffer = new char[100];

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::sprintf(buffer, "%d,%d  %d,%d,%d  %d\n", timer1Msg, timer2Msg, t1Msg, t2Msg, t3Msg, rcvMsg);
        logfile.write(buffer, strlen(buffer));
        logfile.flush();
        // std::cout<<"run...\n";
    }
}


void timerHandler(int signal, siginfo_t *info, void *)
{
    static unsigned int cout = 0;
    Fire &fire = Fire::getInstance();

    fire.getMsgQueue().push(timer1Msg);
    timer1Msg++;

    cout++;
    if(cout % 5 == 0)
    {
        fire.getMsgQueue().push(timer2Msg);
        timer2Msg++;
    }

    if(cout % 10 == 0)
    {
        fire.getMsgQueue().push(timer3Msg);
        timer3Msg++;
    }
}

void timerThread()
{
    std::cout << "timerThread this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "timerThread TID: " << gettid() << std::endl;

    Timer *timer = new Timer(timerHandler, gettid());
    timer->startTimer(10);
    while (1)
    {
        std::this_thread::yield();
    }
}

int main(int argc, char *argv[])
{
    int msg;
    Fire &fire = Fire::getInstance();

    std::thread t1Msg(thread1);
    std::thread t2Msg(thread2);
    std::thread t3Msg(thread3);
    std::thread t4(thread4);
    std::thread tmain(timerThread);

    std::cout << "main TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    fire.run();

    // synchronize threads:
    t1Msg.join();
    t2Msg.join();
    t3Msg.join();
    t4.join();
    tmain.join();

    std::cout << "main exit\n";

    return 0;
}