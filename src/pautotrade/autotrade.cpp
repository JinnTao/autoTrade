#include <future>
#include <chrono>
#include <thread>

#include "cTradingPlatform.h"


extern "C" void signal_handler(int signal) {
    ILOG("Dectect signal:{}", signal);
    global::is_running = false;
}

int main(int32 argc, char** argv) {
    try {
        global::is_running = true;

        logger::initLogger();

        std::signal(SIGTERM, signal_handler);  // program termination
        std::signal(SIGINT, signal_handler);   // interrupt by user

        auto  trader = std::make_unique<cTradingPlatform>();
        int32 result = 0;

        result = trader->loadConfig(TRADE_CONFIG_FILE);
        if (result != 0) {
            WLOG("Trader load config failed! Result:{}", result);
            return -1;
        }
        ILOG("Trader load config success!");

        result = trader->createPath();
        if (result != 0) {
            WLOG("Trader create path failed! Result:{}", result);
            return -2;
        }
        ILOG("Trader create path success!");
        result = trader->init();
        if (result != 0) {
            WLOG("Trader init failed! Result: {}.", result);
            return -3;
        }
        ILOG("Trader init success!");
        result = trader->start();
        if (result != 0) {
            WLOG("Trader start failed! Result:{}.", result);
            return -4;
        }
        // LOG(INFO) << "Trader start success!";

        while (global::is_running) {
            if (global::need_reconnect.load(std::memory_order_relaxed)) {
                auto result = trader->reConnect();
                if (result == 0) {
                    ILOG("Trader reconnect success!");
                }
            }
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10s);
        }
        ILOG("Trader try stop.");
        result = trader->stop();
        logger::releaseLogger();
        ILOG("Trader stop, exited! Result:{}.", result);
        system("pause");
        return result;
    } catch (exception e) {
        WLOG("Error:{}.", e.what());
        exit(1);
    } catch (...) {
        WLOG("Error!");
    }
}