

#include <ratio>
#include <iostream>
#include "mongostore.h"
#include "logger.h"
mongocxx::instance MongoStore::instance_ = {};

MongoStore::~MongoStore() {
    is_running_.store(false, std::memory_order_release);
}

int32 MongoStore::init(const mongoConfig& mongo_config) {
    ILOG("MongoDb Init uri:{},database:{}", mongo_config.address, mongo_config.database);
    try {
        config_        = mongo_config;
        string address = string(mongo_config.address);
        uri_           = {address};
        client_        = {uri_};
        db_            = client_.database(string(mongo_config.database));

    } catch (const std::exception& e) {
        WLOG("MongoDb init failed!Msg:{}.", e.what());
        return -1;
    }

    return 0;
}

int32 MongoStore::start() {
    is_running_.store(true, std::memory_order_release);

    return 0;
}

int32 MongoStore::stop() {
    is_running_.store(false, std::memory_order_release);
    return 0;
}
bool MongoStore::getData(string                                             collectionName,
                         std::chrono::time_point<std::chrono::system_clock> sTimePoint,
                         std::chrono::time_point<std::chrono::system_clock> eTimePoint,
                         std::vector<double>&                                    close,
                         std::vector<double>&                               open,
                         std::vector<double>&                               high,
                         std::vector<double>&                               low,
                         std::vector<int32_t>&                               volume,
                         std::vector<string>&                               dateTime) {
    try {
        using bsoncxx::builder::stream::close_array;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        using bsoncxx::builder::stream::open_array;
        using bsoncxx::builder::stream::open_document;

        using bsoncxx::builder::basic::kvp;

        mongocxx::collection               coll = db_.collection(collectionName);
        bsoncxx::builder::stream::document filter_builder;

        std::chrono::duration<int, std::ratio<60 * 60>> one_hour(1);
        sTimePoint            = sTimePoint + 8 * one_hour;
        eTimePoint            = eTimePoint + 8 * one_hour;
        std::time_t endTime   = std::chrono::system_clock::to_time_t(eTimePoint);
        std::time_t startTime = std::chrono::system_clock::to_time_t(sTimePoint);
        
        //std::cout << " s " << std::ctime(&startTime) << " e " << std::ctime(&endTime) << std::endl;
        filter_builder << "recordTime" << open_document << "$gte" << bsoncxx::types::b_date(sTimePoint) << "$lte"
                       << bsoncxx::types::b_date(eTimePoint) << close_document;
        bsoncxx::builder::stream::document sort_filter;
        sort_filter << "recordTime" << 1;
        mongocxx::options::find out;
        out.sort(sort_filter.view());

        mongocxx::cursor cursor = coll.find(filter_builder.view(), out);

        for (auto&& doc : cursor) {
            close.push_back(doc["close"].get_double());
            high.push_back(doc["high"].get_double());
            low.push_back(doc["low"].get_double());
            open.push_back(doc["open"].get_double());
            volume.push_back(doc["marketVol"].get_int32());
            std::chrono::system_clock::time_point DateTime = doc["recordTime"].get_date();
            std::time_t                           c = std::chrono::system_clock::to_time_t(DateTime - 8 * one_hour);
            string timeS                                    = std::ctime(&c);
            timeS.pop_back();
            dateTime.push_back(timeS);

            ILOG("collectionName:{},{},close:{},high:{},low:{},open:{}",
                 collectionName,
                 dateTime.at(dateTime.size()-1),
                 doc["close"].get_double(),
                 doc["high"].get_double(),
                 doc["low"].get_double(),
                 doc["open"].get_double()
                );
        }
        //LOG(INFO) << " finish import Data";
        return true;

    } catch (const std::exception e) {
        //LOG(INFO) << "Mongo Query faild! " << e.what();
        return false;
    }
}