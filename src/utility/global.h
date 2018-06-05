#pragma once
#include <atomic>
#include<mutex>
namespace global {
    extern std::mutex init_mutex;
    extern std::mutex run_mutex;
    extern std::atomic<bool> need_reconnect;
    
}

