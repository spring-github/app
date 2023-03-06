/*
 * @Author: qiuzhb
 * @Date: 2023-03-03 08:47:46
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 14:37:13
 * @FilePath: /FIRE/common/Timer.h
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */
#include <signal.h>
#include <time.h>
#include <iostream>
#include <mutex>
#include <list>
#include <syscall.h>

class Timer
{

private:
    std::mutex m_signalListMutex;
    typedef std::list<int>::iterator SIG_LIST_IERATOR;
    static std::list<int> m_signalList;
    struct sigevent m_sev;
#define INVALID_SIGNAL -1
    int m_signal;
#define INVALID_TIMER_ID 0
    timer_t m_timerID;
public:
    typedef void (*TIMER_SIGNAL_HANDLER)(int, siginfo_t *, void *);
    Timer(TIMER_SIGNAL_HANDLER handler);
    Timer(TIMER_SIGNAL_HANDLER handler, pid_t signalRcvBy);
    typedef void (*TIMER_THREAD_HANDLER) (union sigval);
    Timer(TIMER_THREAD_HANDLER handler);
    ~Timer();
    int startTimer(int period, bool oneShort = false);
    int stopTimer();
    int getOverRun();
private:
    int allocRealTimeSignal();
    void freeRealTimeSignal();
};

