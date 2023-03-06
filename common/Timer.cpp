
/*
 * @Author: qiuzhb
 * @Date: 2023-03-03 08:47:46
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 15:55:56
 * @FilePath: /FIRE/common/Timer.cpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */

#include "Timer.h"

std::list<int> Timer::m_signalList;


/**
 * @brief:  Upon timer expiration, generate the signal sigev_signo for the process.
        unspecify the pthread id to recv the signal
 * @param {TIMER_SIGNAL_HANDLER} handler
 * @return {*}
 */
Timer::Timer(TIMER_SIGNAL_HANDLER handler)
{
    m_timerID = INVALID_TIMER_ID;
    m_signal = allocRealTimeSignal();
    if (m_signal == INVALID_SIGNAL)
    {
        return;
    }

    /* Establish handler for timer signal */
    std::cout << "Establishing handler for signal " << m_signal << std::endl;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(m_signal, &sa, NULL) == -1)
    {
        std::cout << "sigaction error" << std::endl;
        goto signal_free;
    }


    /*
    SIGEV_SIGNAL: Upon timer expiration, generate the signal sigev_signo for the process.
    unspecify the pthread id to recv the signal
    */
    m_sev.sigev_notify = SIGEV_SIGNAL; 
    m_sev.sigev_signo = m_signal;
    m_sev.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_REALTIME, &m_sev, &m_timerID) == -1) // A settable system-wide real-time clock
    {
        std::cout << "timer_create error" << std::endl;
        goto signal_free;
    }
    printf("timer ID is 0x%lx\n", (long)m_timerID);
    return;

signal_free:
    freeRealTimeSignal();
}

/**
 * @brief: As  for  SIGEV_SIGNAL,  but  the signal is targeted at the thread whose ID is given in
              sigev_notify_thread_id, which must be a thread in the same process as the caller
 * @param {TIMER_SIGNAL_HANDLER} handler
 * @param {pid_t} signalRcvBy: thread id who will process the signal
 * @return {*}
 */
Timer::Timer(TIMER_SIGNAL_HANDLER handler, pid_t signalRcvBy)
{
    m_timerID = INVALID_TIMER_ID;
    m_signal = allocRealTimeSignal();
    if (m_signal == INVALID_SIGNAL)
    {
        return;
    }

    /* Establish handler for timer signal */
    std::cout << "Establishing handler for signal " << m_signal << std::endl;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(m_signal, &sa, NULL) == -1)
    {
        std::cout << "sigaction error" << std::endl;
        goto signal_free;
    }


    m_sev.sigev_notify = SIGEV_THREAD_ID; 
    m_sev._sigev_un._tid = signalRcvBy;    
    m_sev.sigev_signo = m_signal;
    m_sev.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_REALTIME, &m_sev, &m_timerID) == -1) // A settable system-wide real-time clock
    {
        std::cout << "timer_create error" << std::endl;
        goto signal_free;
    }
    printf("timer ID is 0x%lx\n", (long)m_timerID);
    return;

signal_free:
    freeRealTimeSignal();
}

/*
Upon timer expiration, invoke sigev_notify_function
as if it were the  start  function of a new thread.
mong the implementation possibilities  here  are  that  each
timer  notification  could  result  in the creation of a new thread, or that a single
thread is created to receive  all  notifications.
*/
Timer::Timer(TIMER_THREAD_HANDLER handler)
{
    m_timerID = 0;
    m_signal = 0;

    m_sev.sigev_notify = SIGEV_THREAD; // Upon timer expiration, invoke sigev_notify_function as if it were the  start  function of a new thread.
    m_sev.sigev_value.sival_ptr = this;
    m_sev.sigev_notify_function = handler;
    m_sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &m_sev, &m_timerID) == -1) // A settable system-wide real-time clock
    {
        std::cout << "timer_create error" << std::endl;
        return;
    }
    printf("object:0x%lx timer ID is 0x%lx\n", (long)this, (long)m_timerID);
}

Timer::~Timer()
{
    if (m_signal != INVALID_SIGNAL)
        freeRealTimeSignal();
    if (m_timerID != INVALID_TIMER_ID)
        timer_delete(m_timerID);
}

/**
 * @brief: allocate a signal
 * @return {-1: error, > 0: ok}
 */
int Timer::allocRealTimeSignal()
{
    std::unique_lock<std::mutex> lock(m_signalListMutex);
    SIG_LIST_IERATOR it;
    int signal;
    bool finded = 0;

    for (int signal = SIGRTMIN; signal <= SIGRTMAX; signal++)
    {
        for (it = m_signalList.begin(); it != m_signalList.end(); ++it)
        {
            if (signal == *it) // signal alread be used
            {
                break;
            }
        }

        // find a signal not be used
        if (it == m_signalList.end())
        {
            m_signalList.push_back(signal);
            return signal;
        }
    }
    return INVALID_SIGNAL;
}

/**
 * @brief: free a signal
 * @param {int} signal
 * @return {*}
 */
void Timer::freeRealTimeSignal()
{
    if (m_signal == INVALID_SIGNAL)
        return;
    std::unique_lock<std::mutex> lock(m_signalListMutex);
    SIG_LIST_IERATOR it;

    for (it = m_signalList.begin(); it != m_signalList.end(); ++it)
    {
        if (m_signal == *it) // signal finded
        {
            m_signalList.erase(it);
            m_signal = INVALID_SIGNAL;
            std::cout << "exit\n";
            return;
        }
    }
}

/**
 * @brief: startTimer
 * @param {int} period with millisecond
 * @param {bool} oneShort
 * @return {0:ok, -1:NG}
 */
int Timer::startTimer(int period, bool oneShort)
{
    struct itimerspec its;

    if (m_signal == INVALID_SIGNAL || m_timerID == INVALID_TIMER_ID)
        return -1;

    /* Start the timer */
    its.it_value.tv_sec = period / 1000;
    its.it_value.tv_nsec = (period % 1000) * 1000 * 1000;
    if (oneShort)
    {
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
    }
    else
    {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }

    if (timer_settime(m_timerID, 0, &its, NULL) == -1)
    {
        printf("timer_settime");
        return -1;
    }
}

int Timer::stopTimer()
{
    struct itimerspec its;

    if (m_signal == INVALID_SIGNAL || m_timerID == INVALID_TIMER_ID)
        return -1;

    /* Start the timer */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(m_timerID, 0, &its, NULL) == -1)
    {
        printf("timer_settime");
        return -1;
    }
    return 0;
}

/**
 * @brief:  The timer overrun count is the number of additional
       timer expirations that occurred between the time when the signal was generated
       and when it was  deliv‐ ered or accepted
 * @return {*}
 */
int Timer::getOverRun()
{
    if (m_signal == INVALID_SIGNAL || m_timerID == INVALID_TIMER_ID)
        return -1;
    return timer_getoverrun(m_timerID);
}