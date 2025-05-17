#include <iostream>
#include "mine_shared_prt.h"
#include <thread>
#include <vector>
#include <chrono>

void test_mint_shared_ptr_thread_safety() {
    shared_ptr<int> ptr(new int(42));
    std::cout << "[Main] Initial use_count: " << ptr.use_count() << "\n";

    // 创建多个线程，每个线程都增加和减少引用计数
    const int num_threads = 10;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&ptr, i]() -> void {
            std::cout << "[Thread " << i << "] Started.\n";
            for(int j = 0; j < 10000; ++j) {
                shared_ptr<int> local_ptr(ptr);
                if (j % 2000 == 0) { // 每2000次循环打印一次引用计数
                    std::cout << "[Thread " << i << "] Iteration " << j 
                              << ", use_count: " << ptr.use_count() << "\n";
                }
                // 短暂暂停，增加线程切换的可能性
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            std::cout << "[Thread " << i << "] Finished.\n";
        });
    }

    // 等待所有线程完成
    for(auto& thread : threads) {
        thread.join();
    }

    // 检查引用计数是否正确
    std::cout << "[Main] Final use_count: " << ptr.use_count() << "\n";
    if(ptr.use_count() == 1) {
        std::cout << "[Main] Thread safe!\n";
    } else {
        std::cout << "[Main] Thread unsafe!\n";
    }
}

// g++ mine_shared_ptr_test.cc -o my_shared_ptr -lpthread
int main() {
    std::cout << "[Main] Starting test...\n";
    test_mint_shared_ptr_thread_safety();
    std::cout << "[Main] Test completed.\n";
    return 0;
}