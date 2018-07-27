#include <cMdSpi.h>
#include <iostream>
#include <cMarketDataCollection.h>
#include "easylogging++.h"
#include <future>
#include <chrono>
#include "global.h"

using namespace std;



void cMdSpi::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {

    LOG(INFO) << "cMdSpi::OnRspError" ;
    IsErrorRspInfo(pRspInfo);
}

void cMdSpi::OnFrontDisconnected(int nReason) {

    LOG(INFO) << "Md front disconnect!Reason:" << nReason;
    std::lock_guard<std::mutex> guard(mut_);
    if (on_disconnected_fun_) {
        std::invoke(on_disconnected_fun_, nReason);
    }
}

void cMdSpi::OnHeartBeatWarning(int nTimeLapse) {

    cout << "cMdSpi::OnHeartBeatWarning" << nTimeLapse << endl;
}

void cMdSpi::OnFrontConnected() {

    std::lock_guard<std::mutex> guard(mut_);
    LOG(INFO) << "Md connected to front";
    if (on_connected_fun_) {
        std::invoke(on_connected_fun_);
    }
}



void cMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin,
                            CThostFtdcRspInfoField*      pRspInfo,
                            int                          nRequestID,
                            bool                         bIsLast) {
    std::lock_guard<std::mutex> guard(mut_);
    if (pRspUserLogin && !IsErrorRspInfo(pRspInfo)) {

        LOG(INFO) << "OnRspUserLogin Success! TradeDate: " << pRspUserLogin->TradingDay
                  << " SessionId: " << pRspUserLogin->SessionID << " FrontID: " << pRspUserLogin->FrontID;
    }
    if (bIsLast) {
        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(500ms);
        if (on_login_fun_) {
            std::invoke(on_login_fun_, pRspUserLogin, pRspInfo);
        }
    };
}

void cMdSpi::SubscribeMarketData(char* instIdList) {
    vector<char*> list;

    char* token = strtok(instIdList, ",");

    while (token != NULL) {

        list.push_back(token);

        token = strtok(NULL, ",");
    }

    size_t len = list.size();

    char** pInstId = new char*[len];

    for (unsigned int i = 0; i < len; i++) {
        pInstId[i] = list[i];  // vector list 转到 char **
    }

    int ret = ctpmdapi_->SubscribeMarketData(pInstId, len);
    LOG(INFO) << "SubscribeMarketData,Inst: " << pInstId[0] << " result: " << ret;
}
void cMdSpi::SubscribeMarketData(string inst) {
    char p[50];

    memset(p, 0, 50);
    strcpy_s(p, sizeof(inst), inst.c_str());
    SubscribeMarketData(p);
}

void cMdSpi::SubscribeMarketData(shared_ptr<vector<string>> instList) {
    for (auto it = instList->begin(); it != instList->end(); it++) {
        char instBuff[50];
        strcpy(instBuff, it->c_str());
        this->SubscribeMarketData(instBuff);
    }
}
void cMdSpi::SubscribeForQuoteRsp() {
    /*int iResult = m_pUserMdApi->SubscribeForQuoteRsp( ppInstrumentID, m_instrumentIDs.getSize() );
    cout << "--->>> cMdSpi::SubscribeForQuoteRsp: " << ( ( iResult == 0 ) ? "Success" : "Fail") << endl;*/
}

void cMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                CThostFtdcRspInfoField*            pRspInfo,
                                int                                nRequestID,
                                bool                               bIsLast) {
    if (bIsLast && !IsErrorRspInfo(pRspInfo)) {
        // char message[256];
        // sprintf( message, "%s:called cMdSpi::OnRspSubMarketData:%s", cSystem::GetCurrentTimeBuffer().c_str(),
        // pSpecificInstrument->InstrumentID );  cout << message << endl;  if( m_genLog )
        //{
        //    if( m_outputDirectory.IsBlankString() )
        //        cSystem::WriteLogFile( m_logFile.c_str(), message, false );
        //    else
        //    {
        //        cString folderDir = m_outputDirectory + m_logFileFolder + "//";
        //        cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
        //    }
        //}
    }
}

void cMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                 CThostFtdcRspInfoField*            pRspInfo,
                                 int                                nRequestID,
                                 bool                               bIsLast) {
    cout << "--->>> cMdSpi::OnRspSubForQuoteRsp" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)) {
    }
}

void cMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                  CThostFtdcRspInfoField*            pRspInfo,
                                  int                                nRequestID,
                                  bool                               bIsLast) {
    cout << "--->>> cMdSpi::OnRspUnSubMarketData" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)) {
    }
}

void cMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                   CThostFtdcRspInfoField*            pRspInfo,
                                   int                                nRequestID,
                                   bool                               bIsLast) {
    cout << "--->>> cMdSpi::OnRspUnSubForQuoteRsp" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)) {
    }
}

void cMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) {
    // copy right Yiran Yang
    m_pMktDataCollection->OnRtnDepthMarketData(pDepthMarketData);

    position_collection_->update(pDepthMarketData);
}

void cMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField* pForQuoteRsp) {
    cout << "--->>> cMdSpi::OnRtnForQuoteRsp" << endl;
}

bool cMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo) {
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult) {
        
        LOG(INFO) << "cMdSpi:IsErrorRspInfo, ErrorId: " << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg;
    }
    return bResult;
}

void cMdSpi::RegisterMarketDataCollection(cMarketDataCollection* pMktDataCollection) {
    m_pMktDataCollection = pMktDataCollection;

}

void cMdSpi::RegisterPositionCollection(cPositionCollectionPtr p) {
    if (position_collection_.get()) {
        position_collection_.reset();
    }
    position_collection_ = p;
}

int32 cMdSpi::init(const ctpConfig& ctp_config) {
    using namespace std::chrono_literals;
    // 1.create md api instance
    {

        auto mdapi = CThostFtdcMdApi::CreateFtdcMdApi(ctp_config.md_flow_path_);
        if (mdapi == nullptr) {
            LOG(ERROR) << "Md create instance failed!";
            return -1;
        }
        // unique_ptr->ctp's document release call Release api,maybe release then join 2018/07/11 JinnTao
        ctpmdapi_ = {mdapi, [](CThostFtdcMdApi* mdapi) {
                         if (mdapi != nullptr) {
                             mdapi->RegisterSpi(NULL);
                             mdapi->Release();
                         }
                         LOG(INFO) << "Release mdapi";
                     }};
        ctpmdapi_->RegisterSpi(this);
        LOG(INFO) << "Md create instance success!";
    }

    // 2.connect to md Front
    {
        this->clearCallBack();
        ctpmdapi_->RegisterFront(const_cast<char*>(ctp_config.mdAddress));
        std::promise<bool> connect_result;
        std::future<bool>  is_connected = connect_result.get_future();
        on_connected_fun_               = [&connect_result] { connect_result.set_value(true); };
        ctpmdapi_->Init();
        auto wait_result = is_connected.wait_for(3s);
        if (wait_result != std::future_status::ready || is_connected.get() != true) {
            return -2;
        }
        LOG(INFO) << "Md connect front success!";
    }

    // 3.login to md.
    {
        this->clearCallBack();
        std::promise<bool> login_result;
        std::future<bool>  is_logined = login_result.get_future();
        on_login_fun_ = [&login_result](CThostFtdcRspUserLoginField* login, CThostFtdcRspInfoField* info) {
            if (info->ErrorID == 0) {
                login_result.set_value(true);
            } else {
                login_result.set_value(false);
            }
        };
        CThostFtdcReqUserLoginField req;

        memset(&req, 0, sizeof(req));
        strcpy_s(req.BrokerID, sizeof(TThostFtdcBrokerIDType), ctp_config.brokerId);
        strcpy_s(req.UserID, sizeof(TThostFtdcInvestorIDType), ctp_config.userId);
        strcpy_s(req.Password, sizeof TThostFtdcPasswordType, ctp_config.passwd);
        ctp_config_ = ctp_config;

        // Try login
        auto req_login_result = ctpmdapi_->ReqUserLogin(&req, ++request_id_);
        if (req_login_result != 0) {
            LOG(ERROR) << "Md request login failed!";
            return -3;
        }
        auto wait_result = is_logined.wait_for(5s);
        if (wait_result != std::future_status::ready || is_logined.get() != true) {
            return -3;
        }
        LOG(INFO) << "Md login success!";
    }

    // 4.set callback
    {
        this->clearCallBack();
        global::need_reconnect.store(false,
                                     std::memory_order_release);  // current write/read cannot set this store back;
        on_disconnected_fun_ = [](int32 reason) {
            LOG(INFO) << "Md disconnect,try reconnect! reason:" << reason;
            global::need_reconnect.store(true, std::memory_order_release);
        };
    }
    return 0;
}
int32 cMdSpi::stop() {
    if (ctpmdapi_) {
        ctpmdapi_.reset(nullptr);
    }

    return 0;
}

int32 cMdSpi::reConnect(const ctpConfig& ctp_config) {
    return 0;
}
int32 cMdSpi::start() {
    LOG(INFO) << "Md start success!";
    return 0;
}

void cMdSpi::clearCallBack() {
    std::lock_guard<std::mutex> guard(mut_);
    on_connected_fun_    = {};
    on_disconnected_fun_ = {};
    on_login_fun_        = {};
}
void cMdSpi::clear() {
    if (ctpmdapi_) {
        ctpmdapi_.reset(nullptr);
    }
}