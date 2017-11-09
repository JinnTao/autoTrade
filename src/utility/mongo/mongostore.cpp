#include "mongostore.h"
#include "easylogging++.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

// initial easylogging
//INITIALIZE_EASYLOGGINGPP


mongocxx::instance MongoStore::instance_ = {};

MongoStore::~MongoStore() {
    is_running_.store(false, std::memory_order_release);
}

int32 MongoStore::init(const mongoSetting& mongo_config) {
    try {
        config_ = mongo_config;
        string address = string(mongo_config.address);
        uri_ = {address};
        client_ = {uri_};
        db_     = client_.database(string(mongo_config.database));
    } catch (const std::exception& e) {
        LOG(INFO)<< "MongoDb init failed! "<< e.what();
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
bool MongoStore::getData(string collectionName,std::chrono::time_point<std::chrono::system_clock> sTimePoint,std::chrono::time_point<std::chrono::system_clock> eTimePoint, vector<double> &close, vector<double> &open, vector<double> &high, vector<double> &low, vector<double> &volume, vector<string> &dateTime) {
    try {
        using bsoncxx::builder::stream::close_array;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        using bsoncxx::builder::stream::open_array;
        using bsoncxx::builder::stream::open_document;
        
        using bsoncxx::builder::basic::kvp;

        mongocxx::collection coll = db_.collection(collectionName);
        bsoncxx::builder::stream::document filter_builder;
        
        
        filter_builder << "recordTime" << open_document << "$gte" << bsoncxx::types::b_date(sTimePoint) << "$lte" << bsoncxx::types::b_date(sTimePoint) << close_document <<  finalize;
        //filter_builder.view()
        //cout << bsoncxx::to_json(filter_builder);
        bsoncxx::builder::stream::document sort_filter;
        sort_filter << "recordTime" << 1 << finalize;
        mongocxx::options::find out;
        out.sort(sort_filter.view());
        
        mongocxx::cursor cursor = coll.find(filter_builder.view());
        for (auto&& doc : cursor) {
            double closePrice = doc["close"].get_double();
            cout << doc["actionDate"].get_utf8().value << " " << doc["actionTime"].get_utf8().value << " " << closePrice << endl;
            
            //std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        return true;
    
    }
    catch (const std::exception e) {
        LOG(INFO) << "Mongo Query faild! " << e.what();
        return false;
    }
}


//void MongoStore::loop() {
//    while (is_running_.load(std::memory_order_relaxed)) {
//        process();
//    }
//}
//
//void MongoStore::process() {
//    auto count = buffer_.read_available();
//    if (count == 0) {
//        // should yield?
//        std::this_thread::yield();
//        return;
//    }
//    DLOG("MongoDb {} data", count);
//    MarketData data;
//    while (buffer_.pop(data)) {
//        DLOG("MongoDb pop data");
//        try {
//            using bsoncxx::builder::basic::kvp;
//            bsoncxx::builder::basic::document builder{};
//            builder.append(kvp("id", data.instrument_id));
//            builder.append(kvp("actionDate", data.action_day));
//            builder.append(kvp("actionTime", data.action_time));
//            builder.append(kvp("exchange", data.exchange_id));
//            builder.append(kvp("high", data.high));
//            builder.append(kvp("close", (data.close)));
//            builder.append(kvp("open", (data.open)));
//            builder.append(kvp("low", (data.low)));
//            builder.append(kvp("volume", (data.volume)));
//            builder.append(kvp("BidVolume1", (data.bid_volume1)));
//            builder.append(kvp("AskVolume1", (data.ask_volume1)));
//            builder.append(kvp("mdTradingDay", data.md_trading_day));
//            builder.append(kvp("mdUpdateTime", data.md_update_time));
//            builder.append(kvp("recordTime", bsoncxx::types::b_date(data.last_record_time)));
//
//            db_[data.destination_id].insert_one(builder.view());
//            DLOG("MongoDb record one data ok!");
//        } catch (const std::exception& e) {
//            ELOG("MongoDb insert failed! {}", e.what());
//        }
//    }
//}
