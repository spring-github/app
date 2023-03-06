/*
 * @Author: qiuzhb
 * @Date: 2023-02-28 15:22:33
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 20:50:52
 * @FilePath: /FIRE/Fire.cpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */
#include<thread>
#include "Fire.h"

extern int rcvMsg;

Fire &Fire::getInstance()
{
    static Fire instance;
    return instance;
}

MessageQueue<int>& Fire::getMsgQueue()
{
    return m_msgQueue;
}

int Fire::run()
{
    int msg;

    while (1)
    {
        m_msgQueue.wait(msg);
        rcvMsg = msg;
        // if(msg >= 500000000)
        if(msg % 100 == 0)
        std::cout << "recv:" << msg << std::endl;
    }

    while (1)
    {
        if(m_msgQueue.poll(msg))
        {
            std::cout << "poll recv:" << msg << std::endl;
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Fire exit\n";

    return 0;
}



