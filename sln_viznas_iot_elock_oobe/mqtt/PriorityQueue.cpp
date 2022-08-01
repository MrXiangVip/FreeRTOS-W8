//
// Created by wszgx on 2022/8/1.
//

#include "PriorityQueue.h"
#include "fsl_log.h"
#include "stdlib.h"
#include "FreeRTOS.h"

//template<class MqttCmd>
PriorityQueue::PriorityQueue()
{
    data = (MqttCmd*) pvPortMalloc((Capacity + 1)*sizeof(MqttCmd));
    if (!data)
    {
        LOGE("Allocate dynamic memory\r\n");
        return;
    }

    size = 0;
}

//template<class MqttCmd>
PriorityQueue::~PriorityQueue()
{
    while (!Empty())
        Pop();
}

//template<class MqttCmd>
//判空
bool PriorityQueue::Empty()
{
    if (size > 0)
        return false;
    return true;
}

//template<class MqttCmd>
//清空
void PriorityQueue::Clear()
{
    while (!Empty())
        Pop();
}

//template<class MqttCmd>
//判满
bool PriorityQueue::Full()
{
    if (size == Capacity)
        return true;
    return false;
}

//template<class MqttCmd>
//大小
int PriorityQueue::Size()
{
    return size;
}

//template<class MqttCmd>
//入队
void PriorityQueue::Push(MqttCmd *key)
{
    // 空则直接入队  不能省略
    if (Empty())
    {
        data[++size] = *key;
        return;
    }

    int i;

    if (Full())
    {
        LOGE("Priority queue is full\n");
        return;
    }

    for (i = ++size; data[i / 2] > *key; i /= 2)
        data[i] = data[i / 2];
    data[i] = *key;

    //if (key != data[i / 2])          //如果不能插入重复值 用下面的
    //  data[i] = key;
    //else
    //{
    //  size--;
    //  LOGE("Same value");
    //}

}

//template<class MqttCmd>
//出队
void PriorityQueue::Pop()
{
    int i, child;
    MqttCmd *min, *last;

    if (Empty())
    {
        LOGE("Empty queue\n");
        return;
    }

    *min = data[1];
    *last = data[size--];

    for (i = 1; i * 2 <= size; i = child)
    {
        child = i * 2;
        if (child != size && data[child + 1] < data[child])
            child++;

        if (*last > data[child])
            data[i] = data[child];
        else
            break;
    }
    data[i] = *last;
}

//template<class MqttCmd>
//队首
MqttCmd PriorityQueue::Top()
{
    if (Empty())
    {
        LOGE("Priority queue is empty\n");
        return data[0];
    }
    return data[1];
}

