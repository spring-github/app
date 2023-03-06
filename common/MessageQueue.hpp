/*
 * @Author: qiuzhb
 * @Date: 2023-03-01 20:29:28
 * @LastEditors:  
 * @LastEditTime: 2023-03-06 15:58:26
 * @FilePath: /FIRE/common/MessageQueue.hpp
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */
#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
                #include<unistd.h>

template <class T>
class MessageQueue
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;

public:
    MessageQueue(/* args */)
    {
    }
    ~MessageQueue()
    {
    }

    void push(const T &msg)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(msg);
        m_cv.notify_one();
    }

    void wait(T &msg)
    {
        // unique_lock guarantees an unlocked status on destruction (even if not called explicitly). 
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_queue.size())
        {
            m_cv.wait(lock);
        }
        msg = m_queue.front();
        m_queue.pop();
    }

    bool poll(T &msg)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.size())
        {
            msg = m_queue.front();
            m_queue.pop();
            return true;
        }
        else
        {
            return false;
        }
    }
    size_t size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_queue.size();
    }
};

#endif