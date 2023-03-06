/*
 * @Author: qiuzhb
 * @Date: 2023-03-06 10:47:35
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 11:02:48
 * @FilePath: /FIRE/timerTest.cpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

timer_t timerid;
struct sigevent sev;
struct itimerspec its;
long long freq_nanosecs;
sigset_t mask;
struct sigaction sa;

void timer_thread(union sigval arg)
{
    printf("Timer expired\n");
}
void timer_thread2(int signal, siginfo_t *val, void *ct)
{
    printf("thread 2 expired\n");
}

int main(int argc, char argv[])
{
    /* Establish handler for timer signal */
    printf("Establishing handler for signal %!d(MISSING)\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = (void(*)(int, siginfo_t *, void *))timer_thread;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        printf("Failed to establish handler for signal %!d(MISSING)\n", SIG);

    /* Create the timer */
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_thread;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCKID, &sev, &timerid) == -1)
        printf("timer_create");

    /* Start the timer */
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 10000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
        printf("timer_settime");

    /* Sleep for a while; meanwhile the timer may expire
       multiple times */
    while (1)
    {
        printf("run...\n");
        sleep(2);
    }
    return 0;
}