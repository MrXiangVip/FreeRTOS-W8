//
// Created by wszgx on 2022/8/1.
//

#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

#include "mqtt_cmd.h"

//template<class T>
class PriorityQueue
{
private:
    int Capacity = 100;    //队列容量
    int size;                 //队列大小
    MqttCmd* data;             //队列变量

public:
    PriorityQueue();
    ~PriorityQueue();

    int Size();
    bool Full();   //判满
    bool Empty(); //判空
    void Push(MqttCmd *key); //入队
    void Pop();//出队
    void Clear(); //清空
    MqttCmd Top();//队首
};

#endif //_PRIORITYQUEUE_H_
