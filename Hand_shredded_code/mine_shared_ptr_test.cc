#include <iostream>
#include "../Hand_shredded_code/mine_shared_prt.h"
#include <thread>
#include <vector>
#include <chrono>

void test_mint_shared_ptr_thread_safety() {
    shared_ptr<int> ptr(new int(42));

    // 创建多个线程， 每个线程都增加和减少引用计数
    const int num_threads = 10;
    std::vector<std::thread> threads;
    for(int i = 0 ; i < num_threads; ++i) {
        threads.emplace_back([&ptr]() -> void {
            for(int j = 0 ; j < 10000 ; ++j) {
                shared_ptr<int> local_ptr(ptr);
                // 短暂暂停，增加线程切换的可能性
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    // 等待所有线程完成
    for(auto& thread : threads) {
        thread.join();
    }

    // 检查引用计数是否正确
    std::cout << "use_count" << ptr.use_count() << "\n";
    if(ptr.use_count() == 1) {
        std::cout << "thread safe!" << "\n";
    } else {
        std::cout << "thread unsafe!" << "\n";
    }
}

// g++ mine_shared_ptr_test.cc -o -l pthread
int main() {
    test_mint_shared_ptr_thread_safety();
}