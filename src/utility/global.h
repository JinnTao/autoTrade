#pragma once
#include <atomic>
#include <mutex>
#include <csignal>
namespace global {
    extern std::mutex init_mutex;
    extern std::mutex run_mutex;
    extern std::atomic<bool> need_reconnect;
    extern volatile std::sig_atomic_t is_running;
}

