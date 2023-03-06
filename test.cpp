/*
 * @Author: qiuzhb
 * @Date: 2023-02-28 15:23:40
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 14:56:50
 * @FilePath: /FIRE/test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */

#include <thread>
#include "common/MessageQueue.hpp"
#include "common/Timer.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

MessageQueue<int> msgQueue;
std::recursive_mutex mutex;

#include <syscall.h>
pid_t gettid()
{
    return syscall(SYS_gettid);
}

void thread1()
{
    int msg = 0;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "thread1 id:" << std::this_thread::get_id() << std::endl;
    std::cout << "TID1: " << gettid() << std::endl;
    while (1)
    {
        msgQueue.push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void thread2()
{
    int msg = 100000000;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "thread2 id:" << std::this_thread::get_id() << std::endl;
    std::cout << "TID2: " << gettid() << std::endl;

    while (1)
    {
        msgQueue.push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void thread3()
{
    int msg = 500000000;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "thread3 id:" << std::this_thread::get_id() << std::endl;
    std::cout << "TID3: " << gettid() << std::endl;

    while (1)
    {
        msgQueue.push(msg);
        msg++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void timerAHandler(int signal, siginfo_t *info, void *)
{
    // Timer *timer = (Timer *)info->si_value.sival_ptr;
    // std::cout << "signal " << signal << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;
    // // std::cout << "overrun:" << timer->getOverRun() << std::endl;

    // std::unique_lock<std::recursive_mutex> lock(mutex);
    //     if (lock.owns_lock())
    //     std::cout << "A lock " << std::this_thread::get_id() << std::endl;
    // else
    //     std::cout << "A unlock " << std::this_thread::get_id() << std::endl;

    std::cout << "timerA this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "timerA TID: " << gettid() << std::endl;
}

void timerBHandler(int signal, siginfo_t *info, void *)
{
    // Timer *timer = (Timer *)info->si_value.sival_ptr;
    // std::cout << "signal " << signal << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;

    // std::unique_lock<std::recursive_mutex> lock(mutex);
    // if (lock.owns_lock())
    //     std::cout << "B lock " << std::this_thread::get_id() << std::endl;
    // else
    //     std::cout << "B unlock " << std::this_thread::get_id() << std::endl;

    std::cout << "timerB this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "timerB TID: " << gettid() << std::endl;
}

void timerCHandler(int signal, siginfo_t *info, void *)
{
    // Timer *timer = (Timer *)info->si_value.sival_ptr;
    // std::cout << "signal " << signal << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;

    // std::unique_lock<std::recursive_mutex> lock(mutex);
    // if (lock.owns_lock())
    //     std::cout << "C lock " << std::this_thread::get_id() << std::endl;
    // else
    //     std::cout << "C unlock " << std::this_thread::get_id() << std::endl;

    std::cout << "timerC this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "timerC TID: " << gettid() << std::endl;
}

int timer1thread = 0;
int timer2thread = 0;
int timer3thread = 0;

void timerThread1(union sigval val)
{
    // std::unique_lock<std::recursive_mutex> lock(mutex);

    Timer *timer = (Timer *)val.sival_ptr;
    // std::cout << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;
    // std::cout << " thread " << std::this_thread::get_id() << std::endl;
    timer1thread++;

    std::cout << "tm1 this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "pthread ID: " << pthread_self() << std::endl;
    std::cout << "TID: " << gettid() << std::endl;
}

void timerThread2(union sigval val)
{
    // std::unique_lock<std::recursive_mutex> lock(mutex);

    Timer *timer = (Timer *)val.sival_ptr;
    // std::cout << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;
    // std::cout << " thread " << std::this_thread::get_id() << std::endl;
    timer2thread++;

    std::cout << "tm2 this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "pthread ID: " << pthread_self() << std::endl;
    std::cout << "TID: " << gettid() << std::endl;
}

void timerThread3(union sigval val)
{
    // std::unique_lock<std::recursive_mutex> lock(mutex);
    Timer *timer = (Timer *)val.sival_ptr;
    // std::cout << " object " << timer << " thread " << std::this_thread::get_id() << std::endl;
    // std::cout << " thread " << std::this_thread::get_id() << std::endl;
    timer3thread++;

    std::cout << "tm3 this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "pthread ID: " << pthread_self() << std::endl;
    std::cout << "TID: " << gettid() << std::endl;
}

void timerMain()
{

    std::cout << "timerMain this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "timerMain TID: " << gettid() << std::endl;

    Timer *timerA = new Timer(timerAHandler, gettid());
    Timer *timerB = new Timer(timerBHandler, gettid());
    Timer *timerC = new Timer(timerCHandler, gettid());
    timerA->startTimer(10);
    timerB->startTimer(10);
    timerC->startTimer(10);

    while (1)
    {
        std::this_thread::yield();
    }
}

int main(int argc, char *argv[])
{

    // Timer *timerA = new Timer(timerAHandler);
    // Timer *timerB = new Timer(timerBHandler);
    // Timer *timerC = new Timer(timerCHandler);
    // // Timer *timerA = new Timer(timerThread1);
    // // Timer *timerB = new Timer(timerThread2);
    // // Timer *timerC = new Timer(timerThread3);
    // // timerA->startTimer(1000);
    // // timerB->startTimer(1000);
    // // timerC->startTimer(1000);

    // int i = 0;
    // while (1)
    // {
    //     {
    //         // std::unique_lock<std::recursive_mutex> lock(mutex);

    //         std::cout << "main this_thread id:" << std::this_thread::get_id() << std::endl;
    //         std::cout<<"main TID: " << gettid() << std::endl;

    //         // std::cout<<timer1thread<<" "<<timer2thread<<" "<<timer3thread<<" \n";
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         i++;
    //     }

    // }

    // // while (1)
    // // {
    // //     if (lock.owns_lock())
    // //         std::cout << "main lock " << std::this_thread::get_id() << std::endl;
    // //     else
    // //         std::cout << "main unlock " << std::this_thread::get_id() << std::endl;

    // //     sleep(1);
    // // }

    // timerA->startTimer(1000, true);
    // timerB->startTimer(100);
    // std::cout << "timerA:" << timerA << std::endl;
    // std::cout << "timerB:" << timerB << std::endl;

    // std::cout << "sleep_for" << std::endl;
    // // sleep_for will block although the thread receive a signal
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // std::cout << "sleep" << std::endl;
    // sleep(500000);

    // timerA->startTimer(100);

    // std::cout << "sleep_for" << std::endl;
    // // sleep_for will block although the thread receive a signal
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // timerA->stopTimer();
    // std::cout << "sleep_for" << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // delete timerA;
    // delete timerB;

    // return 0;

    int msg;

    std::cout << "main this_thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "main TID: " << gettid() << std::endl;
    std::thread t1(thread1);
    std::thread t2(thread2);
    std::thread t3(thread3);
    std::thread t4(timerMain);

    while (1)
    {
        msgQueue.wait(msg);
        std::cout << "recv:" << msg << std::endl;
    }

    while (1)
    {
        if (msgQueue.poll(msg))
        {
            std::cout << "poll recv:" << msg << std::endl;
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // synchronize threads:
    t1.join();
    t2.join();
    t3.join();

    std::cout << "exit\n";

    return 0;
}