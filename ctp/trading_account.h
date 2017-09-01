#ifndef __TRADING_ACCOUNT_H__
#define __TRADING_ACCOUNT_H__

#include <stl_ctp.h>


class cTradingAccount
{
public:
	cTradingAccount() {}
	~cTradingAccount() {}

	char* GetQFrontAddress() { return m_qFrontAddress; };
	char* GetTFrontAddress() { return m_tFrontAddress; }
	char* GetBrokerID()		 { return m_brokerID; }
	char* GetInvestorID()	 { return m_investorID; }
	char* GetPassword()	     { return m_password; }

private:
	char* m_qFrontAddress;
	char* m_tFrontAddress;
	char* m_brokerID;
	char* m_investorID;
	char* m_password;

};

#define SETTINGFILE "../config/setting.json"

#define STRATEGYFILE "../config/Strategy.json"


// demo-trading account
#ifndef QFRONT_ADDR_DEMO
#define QFRONT_ADDR_DEMO "tcp://180.169.30.167:41213"
#endif

#ifndef TFRONT_ADDR_DEMO
#define TFRONT_ADDR_DEMO "tcp://180.168.163.198:7001"
#endif

#ifndef BROKER_ID_DEMO
#define BROKER_ID_DEMO "RohanDemo"
#endif

#ifndef INVESTOR_ID_DEMO
#define INVESTOR_ID_DEMO "jjtt01"
#endif

#ifndef PASSWORD_DEMO
#define PASSWORD_DEMO "888888"
#endif

// real-trading account trading
#ifndef QFRONT_ADDR
#define QFRONT_ADDR "tcp://27.115.57.134:41213"
#endif

#ifndef TFRONT_ADDR
#define TFRONT_ADDR "tcp://27.115.57.134:41205"
#endif

#ifndef BROKER_ID
#define BROKER_ID "9000"
#endif

#ifndef INVESTOR_ID
#define INVESTOR_ID "880819988"
#endif

#ifndef PASSWORD
#define PASSWORD "095759"
#endif




//char QFRONT_ADDR_DEMO[] = "tcp://180.168.146.187:10010";		// front address
//char TFRONT_ADDR_DEMO[] = "tcp://180.168.146.187:10001";		// front address
//TThostFtdcBrokerIDType	BROKER_ID_DEMO = "9999";				// broker ID
//TThostFtdcInvestorIDType INVESTOR_ID_DEMO = "036762";		// investor ID
//TThostFtdcPasswordType  PASSWORD_DEMO = "attention3";		// user password



// real-trading account trading
// configurations
//char QFRONT_ADDR[] = "tcp://27.115.57.134:41213";		// front address
//char TFRONT_ADDR[] = "tcp://27.115.57.134:41205";		// front address
//TThostFtdcBrokerIDType	BROKER_ID = "9000";				// broker ID
//TThostFtdcInvestorIDType INVESTOR_ID = "880819988";		// investor ID
//TThostFtdcPasswordType  PASSWORD = "095759";            // user password


#endif


