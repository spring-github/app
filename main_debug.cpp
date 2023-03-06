/*
 * @Author: qiuzhb
 * @Date: 2023-02-28 15:23:40
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 20:36:31
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
int t1Msg = 0;
int t2Msg = 0;
int t3Msg = 0;
int rcvMsg = 0;
int timer1 = 400000000;
int timer2 = 500000000;
int timer3 = 600000000;

std::thread::id mainID, t1ID, t2ID, t3ID, t4ID, t5ID;
int t1ISR = 0;
int t2ISR = 0;
int t3ISR = 0;
int t4ISR = 0;
int t5ISR = 0;
int mainISR = 0;

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
    // blockSignal();

    int msg = 100000000;
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread1 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    t1ID = std::this_thread::get_id();
    // std::unique_lock<std::mutex> lock(m);
    while (1)
    {
        // std::cout << "thread1 " << msg << "\n";
        t1Msg = msg;
        fire.getMsgQueue().push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread2()
{
    // blockSignal();

    int msg = 200000000;
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread2 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    // std::unique_lock<std::mutex> lock(m);
    t2ID = std::this_thread::get_id();

    while (1)
    {
        // std::cout << "thread2 " << msg << "\n";
        t2Msg = msg;
        fire.getMsgQueue().push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread3()
{
    // blockSignal();

    int msg = 300000000;
    Fire &fire = Fire::getInstance();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread3 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    // std::unique_lock<std::mutex> lock(m);
    t3ID = std::this_thread::get_id();

    while (1)
    {
        // std::cout << "thread3 " << msg << "\n";
        t3Msg = msg;
        fire.getMsgQueue().push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void thread4()
{
    // blockSignal();

    std::cout << "thread4 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    t4ID = std::this_thread::get_id();

    std::ofstream logfile("log.txt", std::ofstream::binary);
    std::ofstream outfile("ISR.txt", std::ofstream::binary);
    // allocate memory for file content
    char *buffer = new char[100];

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::sprintf(buffer, "%d,%d  %d,%d,%d  %d\n", timer1, timer2, t1Msg, t2Msg, t3Msg, rcvMsg);
        logfile.write(buffer, strlen(buffer));
        logfile.flush();

        std::sprintf(buffer, "%d,%d,%d,%d,%d,%d\n", mainISR, t1ISR, t2ISR, t3ISR, t4ISR, t5ISR);
        outfile.write(buffer, strlen(buffer));
        outfile.flush();

        // std::cout<<"run...\n";
    }
}

void thread5()
{

    std::cout << "thread5 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
    t5ID = std::this_thread::get_id();

    while (1)
    {
        std::cout << "thread5 TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";
        sleep(5);
    }
}

void timerHandler(int signal, siginfo_t *info, void *)
{
    static unsigned int cout = 0;
    Fire &fire = Fire::getInstance();

    fire.getMsgQueue().push(timer1);
    timer1++;

    if(cout % 5 == 0)
    {
        fire.getMsgQueue().push(timer2);
        timer2++;
    }

    if(cout % 10 == 0)
    {
        fire.getMsgQueue().push(timer3);
        timer3++;
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
    // std::unique_lock<std::mutex> lock(m);

    // std::recursive_mutex test;
    // std::unique_lock<std::recursive_mutex> testlock(test);
    // std::unique_lock<std::recursive_mutex> testlock2(test);
    // testlock.unlock();
    // testlock2.unlock();

    // Timer a(timerHandler);
    // Timer b(timerBHandler);

    std::cout << "main TID:" << gettid() << " thread:" << std::this_thread::get_id() << "\n";

    mainID = std::this_thread::get_id();
    std::thread t1Msg(thread1);
    std::thread t2Msg(thread2);
    std::thread t3Msg(thread3);
    std::thread t4(thread4);
    std::thread t5(thread5);
    std::thread tmain(timerThread);

    // blockSignal();

    // sleep(3);
    // a.startTimer(10);
    // b.startTimer(20);

    fire.run();

    // synchronize threads:
    t1Msg.join();
    t2Msg.join();
    t3Msg.join();

    std::cout << "main exit\n";

    return 0;
}