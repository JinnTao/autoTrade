#include "global.h"

namespace global {

    std::atomic<bool> need_reconnect;
    std::mutex init_mutex;
    std::mutex run_mutex;
    volatile std::sig_atomic_t is_running;

}  // namespace global
