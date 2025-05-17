#include "mine_timer.h"
#include <ctime>
#include <memory>
#include <sys/epoll.h>


// g++ mine_timer_test.cc -o 
int main() {
    int epfd = epoll_create(1);// 构建epoll对象

    unique_ptr<Timer> timer = make_unique<Timer>();// 创建Timer实例

    int i = 0; 

    timer->AddTimeout(1000, [&](TimerTask* task){
        cout << Timer::GetTick() << "addtime: " << task->AddTime() << "revoked times: " << ++i << '\n'; 
    });

    timer->AddTimeout(2000, [&](TimerTask* task){
        cout << Timer::GetTick() << "addtime: " << task->AddTime() << "revoked times: " << ++i << '\n'; 
    });

    timer->AddTimeout(3000, [&](TimerTask* task){
        cout << Timer::GetTick() << "addtime: " << task->AddTime() << "revoked times: " << ++i << '\n'; 
    });

    auto task = timer->AddTimeout(2100, [&](TimerTask* task){
        cout << Timer::GetTick() << "addtime: " << task->AddTime() << "revoked times: " << ++i << '\n'; 
    });
    timer->DelTimeout(task);

    cout << " now time :" << Timer::GetTick() << '\n';
    epoll_event ev[64] = {0};

    while (true) {
        cout << "waitTime: " << timer->WaitTime() << '\n';
        int n = epoll_wait(epfd, ev, 64, timer->WaitTime());
        time_t now = Timer::GetTick();
        for(int i = 0 ; i < n; i++) {
            /**/
        }
        /*处理定时事件*/
        timer->Update(now);
    }
    return 0;
}