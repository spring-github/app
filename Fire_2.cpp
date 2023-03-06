/*
 * @Author: qiuzhb
 * @Date: 2023-03-02 10:37:21
 * @LastEditors:  
 * @LastEditTime: 2023-03-02 10:37:23
 * @FilePath: /FIRE/Fire2.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by nebula, All Rights Reserved. 
 */
class Fire
{
private:
    Fire(){};
    Fire(const Fire &) = delete;
    Fire &operator=(const Fire &) = delete;
    static Fire *instance;

public:
    static Fire *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new Fire();
        }
        return instance;
    }
};

Fire *Fire::instance = nullptr;