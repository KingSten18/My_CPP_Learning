/*
手撕定时器

1、要用什么数据结构？ ---从STL里面寻找
    - 触发时刻为key,执行任务为value
    - 便于寻找到期任务  O(1)
    - 触发任务之后删除任务 
    - 允许相同的时刻触发不同的任务 
    - 有序、便于排序

    - 红黑树 和 最小堆，但是STL中的最小堆不支持删除操作，所以选择红黑树

    那就是  multimap：(红黑树)  找最小值很方便，元素变动之后会自平衡，再找最小值也很方便


2、什么时候执行？
    - 到期之后执行，异步的操作，用回调函数，通过函数对象的方式

3、如何触发任务? 触发之前干什么？
    - 通过通知的方式来触发
    - IO多路复用的方式

4、接口设计：
    - 添加一个延时任务
    - 删除一个延时任务
    - 检测触发延时任务

5、性能优化
    - 针对不同功能、不同对象，设置不同的定时器
    - 针对多线程优化：一个事件线程设置一个定时器，或者单独时间轮线程，有一个线程专门用来检测是否触发，触发之后抛给其他线程执行
    - 针对有规律的定时任务（有大量相同时间间隔的延时任务），采用emplace_hint接口，带提示的插入功能，可以把插入从log2N的复杂度优化到O（1） 


*/
#pragma once

#include <cstdint>
#include <iostream>
#include <functional>
#include <chrono>
#include <map>
#include <memory>
#include <sys/epoll.h>
#include <type_traits>

using namespace std;

class Timer;
class TimerTask {
    friend class Timer; 
    // 友元类：把执行任务设置为私有，防止任务被手动执行
    // 为了能被Timer执行，把Timer设置为友元类
    using Callback = std::function<void(TimerTask *task)>;
public:
    TimerTask(uint64_t addtime, uint64_t exectime, Callback func) {
        m_addTime = addtime;
        m_execTime = exectime;
        m_func = func;
    }

    uint64_t AddTime() const {
        return m_addTime;
    }

    uint64_t ExecTime() const {
        return m_execTime;
    }

private:
    void run() {
        m_func(this);
    }

private:
    uint64_t m_addTime;
    uint64_t m_execTime;
    Callback m_func;
};

class Timer {
    using Milliseconds = std::chrono::milliseconds;

public:
    static uint64_t GetTick() {
        auto sc = chrono::time_point_cast<Milliseconds>(chrono::steady_clock::now());// 系统启动到现在的时刻
        auto temp = chrono::duration_cast<Milliseconds>(sc.time_since_epoch());
        return temp.count();
    }

    // 添加定时器                 多长时间后(ms)               执行这个函数
    TimerTask * AddTimeout(uint64_t offset, TimerTask::Callback func) {
        auto now = GetTick();
        auto exectime = now + offset;
        auto task = new TimerTask(now, exectime, std::move(func));

        // 添加定时器的优化，使用emplace_hint
        if(m_timeouts.empty() || exectime >= m_timeouts.crbegin()->first) {
            auto ele = m_timeouts.emplace_hint(m_timeouts.end(), exectime, task);
            return ele->second;
        }
        auto ele = m_timeouts.emplace(exectime, task);
        return ele->second;
    }

    // 删除定时器
    void DelTimeout(TimerTask* task) {
        auto range = m_timeouts.equal_range(task->ExecTime());
        for(auto itr = range.first; itr != range.second;) {
            if(itr->second == task) {
                itr = m_timeouts.erase(itr);
            } else {
                ++itr;
            }
        }
    }

    // 更新定时器
    void Update(uint64_t now) {
        auto itr = m_timeouts.begin();
        while (itr != m_timeouts.end() && itr->first <= now) {
            itr->second->run();
            delete itr->second;
            itr = m_timeouts.erase(itr);
        }
    }

    // 获取最近的超时时间
    int WaitTime() {
        auto itr = m_timeouts.begin();
        if(itr == m_timeouts.end()) {
            return -1;
        }
        int diss = itr->first - GetTick();
        return diss > 0 ? diss : 0;
    }

private:
    std::multimap<uint64_t, TimerTask*> m_timeouts;

};