

#include <ratio>
#include <iostream>
#include "mongostore.h"
#include "logger.h"
mongocxx::instance MongoStore::instance_ = {};

MongoStore::~MongoStore() {
    is_running_.store(false, std::memory_order_release);
}

int32 MongoStore::init(const mongoConfig& mongo_config) {
    try {
        config_        = mongo_config;
        string address = string(mongo_config.address);
        uri_           = {address};
        client_        = {uri_};
        db_            = client_.database(string(mongo_config.database));
    } catch (const std::exception& e) {
        ILOG("MongoDb init failed!Msg:{}.", e.what());
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
                         std::vector<double>&                               volume,
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
        // document{} << "recordTime" << open_document << "$gte" << bsoncxx::types::b_date(sTimePoint) << "$lte" <<
        // bsoncxx::types::b_date(sTimePoint) << close_document << finalize);
        //LOG(INFO) << "input Data from mongoDb";
        for (auto&& doc : cursor) {
            close.push_back(doc["close"].get_double());
            high.push_back(doc["high"].get_double());
            low.push_back(doc["low"].get_double());
            open.push_back(doc["open"].get_double());
            // volume.push_back(doc["volume"].get_double());

            //LOG(INFO) << doc["actionDate"].get_utf8().value << " " << doc["actionTime"].get_utf8().value << " close "
             //         << doc["close"].get_double().value;
            // std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        //LOG(INFO) << " finish import Data";
        return true;

    } catch (const std::exception e) {
        //LOG(INFO) << "Mongo Query faild! " << e.what();
        return false;
    }
}