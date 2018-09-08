#ifndef __CORDER_H__
#define __COREDR_H__


#include <chrono>
#include <string>
#include <memory>

#include "global.h"
#include "ThostFtdcUserApiStruct.h"

class cOrder {
    using cTickTime = std::chrono::system_clock::time_point;

public:
    cOrder();
    cOrder(CThostFtdcOrderField*);
    cOrder(const cOrder&);
    cOrder& operator=(const cOrder&);

    ~cOrder() {}

    //
    /* Get Method */
    int                   GetOrderID() const { return m_orderID; }
    std::string           GetInstrumentID() const { return m_instrumentID; }
    std::string           GetAccountID() const { return m_accountID; }
    int                   GetOrderRef() const { return m_orderRef; }
    char                  GetDirection() const { return m_direction; }
    double                GetPrice() const { return m_price; }
    char                  GetOffsetFlag() const { return m_offsetFlag; }
    int                   GetVolumeOriginal() const { return m_volumeOriginal; }
    int                   GetVolumeTraded() const { return m_volumeTraded; }
    int                   GetSeqNo() const { return m_BrokerOrderSeq; }
    cTickTime             GetOrderServerTime() const { return m_orderServerTime; }
    cTickTime             GetOrderLocalTime() const { return m_orderLocalTime; }
    CThostFtdcOrderField* getOrderStruct() { return &m_orderField; }
    //
    /* Set Method */
    void SetInstrumentID(const std::string& id) { m_instrumentID = id; }

    void SetDirection(int direction) {
        switch (direction) {
            case 0:
                m_direction = '0';
                break;
            case 1:
                m_direction = '1';
                break;
            default:
                m_direction = '0';
                break;
        }
    }

    void SetOffsetFlag(int flag) {
        switch (flag) {
            case 0:
                m_offsetFlag = '0';
                break;
            case 1:
                m_offsetFlag = '1';
                break;
            case 3:
                m_offsetFlag = '3';
                break;
            case 4:
                m_offsetFlag = '4';
                break;
        }
    }
    void SetVolumeOriginal(int volume) { m_volumeOriginal = volume; }
    void SetVolumeTraded(int volume) { m_volumeTraded = volume; }
    void SetPrice(double price) { m_price = price; }
    void SetOrderTime(const cTickTime& t) { m_orderServerTime = t; }
    void SetOrderID(int id) { m_orderID = id; }

    void Print() const;

    bool IsPendingOrder() const;
    bool IsCancelledOrder() const;

    TThostFtdcOrderSysIDType m_orderSysID;
    TThostFtdcExchangeIDType ExchangeID;

private:
    int         m_orderID;
    std::string m_instrumentID;
    std::string m_accountID;

    int m_orderRef;

    char m_direction;
    char m_offsetFlag;
    char m_status;

    double m_price;
    int    m_volumeOriginal;
    int    m_volumeTraded;

    cTickTime m_orderServerTime;
    cTickTime m_orderLocalTime;

    int                      m_frontID;
    int                      m_sessionID;
    TThostFtdcSequenceNoType m_BrokerOrderSeq;
    CThostFtdcOrderField     m_orderField;
};

typedef std::shared_ptr<cOrder> cOrderPtr;

class cStopOrder {
public:
    std::string           instrument;
    std::string           orderType;
    traderTag::DIRECTION  direction;
    traderTag::OFFSETFLAG offset;

    double       price;
    unsigned int volume;
    std::string  strategyName;
    bool         status;
    size_t       slipTickNum;
    int32        order_id_;

    std::chrono::system_clock::time_point orderTime;

    cStopOrder() {
        instrument   = "";
        orderType    = "";
        direction    = traderTag::DIRECTION::buy;
        offset       = traderTag::OFFSETFLAG::close;
        price        = 0;
        volume       = 0;
        strategyName = "";
        status       = false;
    }
    ~cStopOrder() {}
};

#endif