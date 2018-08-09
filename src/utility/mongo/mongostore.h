#ifndef _MONGOSTORE_H_
#define _MONGOSTORE_H_

#include <atomic>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>

#include "common.h"
#include "global.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

 class MongoStore {
 public:
    using DataBuffer = std::queue<int>;

    MongoStore() = default;
    ~MongoStore();
    int32_t init(const mongoConfig& mongo_config);
    int32_t start();
    int32_t stop();
    bool getData(string collectionName, std::chrono::time_point<std::chrono::system_clock> sTimePoint,
                    std::chrono::time_point<std::chrono::system_clock> eTimePoint,
                    std::vector<double>&                               close,
                    std::vector<double>&                               open,
                    std::vector<double>&                               high,
                    std::vector<double>&                               low,
                    std::vector<double>&                               volume,
                    std::vector<string>&                               dateTime);
 private:
    //void loop();
    //void process();

    static mongocxx::instance instance_;

    std::atomic<bool>    is_running_{ATOMIC_FLAG_INIT};
    mongoConfig          config_;
    mongocxx::uri        uri_;
    mongocxx::client     client_;
    mongocxx::database   db_;
    //std::thread          inter_thread_;

    DISALLOW_COPY_AND_ASSIGN(MongoStore);
};

#endif  // _MONGOSTORE_H_
