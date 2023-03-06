/*
 * @Author: qiuzhb
 * @Date: 2023-02-28 15:22:46
 * @LastEditors:  
 * @LastEditTime: 2023-03-02 10:52:58
 * @FilePath: /FIRE/Fire.h
 * @Description:
 *
 * Copyright (c) 2023 by nebula, All Rights Reserved.
 */
#ifndef FIRE_H
#define FIRE_H
#include "common/MessageQueue.hpp"

class Fire
{
private:
    MessageQueue<int> m_msgQueue;

public:
    static Fire &getInstance();
    int run();
    MessageQueue<int> &getMsgQueue();

private:
    Fire() {}
    Fire(const Fire &);
    Fire &operator=(const Fire &);
    ~Fire() {}
};

#endif