#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async_logger.h"
#include "spdlog/async.h"

namespace logger {

    inline int initLogger() {
        
        auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>();
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_st>("logs/log", 00,00);
        auto combined_logger = std::make_shared<spdlog::logger>("global", spdlog::sinks_init_list{ console_sink, file_sink });
        combined_logger->flush_on(spdlog::level::err);
        combined_logger->set_level(spdlog::level::trace);
        combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l][%t]%^%v%$");
        spdlog::register_logger(combined_logger);

        return 0;
    }

    inline int releaseLogger() {
        spdlog::drop_all();
        return 0;
    }

}  // namespace logger

#define SPDLOG_STR_H(x) #x
#define SPDLOG_STR_HELPER(x) SPDLOG_STR_H(x)
#ifdef _DEBUG
#define DLOG(...) spdlog::get("global")->debug("[" __FILE__ " line #" SPDLOG_STR_HELPER(__LINE__) "]" __VA_ARGS__)
#else
#define DLOG(...)
#endif  // _DEBUG
#define ILOG(...) spdlog::get("global")->info("[" __FILE__ " line #" SPDLOG_STR_HELPER(__LINE__) "]" __VA_ARGS__)
#define WLOG(...) spdlog::get("global")->warn("[" __FILE__ " line #" SPDLOG_STR_HELPER(__LINE__) "]" __VA_ARGS__)
#define ELOG(...) spdlog::get("global")->error("[" __FILE__ " line #" SPDLOG_STR_HELPER(__LINE__) "]" __VA_ARGS__)

#endif  // _LOGGER_H_
