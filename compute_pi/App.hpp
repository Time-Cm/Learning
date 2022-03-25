/**
 * @file App.hpp
 * @author Bilibili@碳-Time
 * @version 0.1
 * @date 2022-02-27
 *
 * @copyright Copyright (c) 2022
 *
 * @details 使用蒙特卡洛方法计算圆周率
 */

#ifndef APP_H
#define APP_H

#include <thread>
#include<ctime>

//以下是用户可修改区
#define THREADS 4
#define COUNTS (int)1e6 //总采样次数
#define POSITION_MAP (long long int)1e6//计算时在正方形区域内随机落点的范围

//以下是核心代码区，不建议修改
class App
{
public:
    double pi(App *myselfPtr = nullptr); //输出函数，若给类分配内存，则需要传入分配到的指针。
private:
    void compute(int* inputData); //多线程的运行函数
};

#endif