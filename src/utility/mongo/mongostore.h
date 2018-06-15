#ifndef _MONGOSTORE_H_
#define _MONGOSTORE_H_

#include <atomic>
#include <queue>
#include <vector>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "autotrade_config.h"
#include "common.h"

using std::vector;

class MongoStore {
public:
    using DataBuffer = std::queue<int>;

    MongoStore() = default;
    ~MongoStore();
    int32_t init(const mongoConfig& mongo_config);
    int32_t start();
    int32_t stop();
    bool getData(string collectionName, std::chrono::time_point<std::chrono::system_clock> sTimePoint, std::chrono::time_point<std::chrono::system_clock> eTimePoint, vector<double> &close, vector<double> &open, vector<double> &high, vector<double> &low, vector<double> &volume, vector<string> &dateTime);
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
