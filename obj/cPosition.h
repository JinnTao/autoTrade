#ifndef __CPOSITION_H__
#define __CPOSITION_H__

#include <cString.h>
#include <cDate.h>
#include <stl.h>
#include <ThostFtdcUserApiStruct.h>

class cTrade;

class cPositionDetail
{
public:
	cPositionDetail(string inst);
	void update( CThostFtdcInvestorPositionField* pInvestorPositionDetail );

	~cPositionDetail() {}
	//
	/* Get Method */
	int GetTradeID() const { return m_tradeID; }
	//cString GetInstrumentID() const { return m_instrumentID; }
	//cString GetAccountID() const { return m_accountID; }
	//double GetPrice() const { return m_price; }
	//int GetVolume() const { return m_volume; }
	//cDate GetOpenDate() const { return m_openDate; }
	//bool IsToday() const { return m_isToday; }
	//char GetDirection() const { return m_direction; }
	int getHoldLong(){return this->m_holding_long;}
	int getHoldShort(){return this->m_holding_short;}
	int getYdLong(){return this->m_YdPosition_long;}
	int getTdLong(){return this->m_TodayPosition_long;}

	int getYdShort(){return this->m_YdPosition_short;}
	int getTdShort(){return this->m_TodayPosition_short;}
	//
	/* Set Method */
	//void SetVolume( int volume ) { m_volume = volume; }

	//
	void Print() ;
	
	double closeProfit_long;//多单平仓盈亏
	
	double OpenProfit_long;//多单浮动盈亏

	double closeProfit_short;//空单平仓盈亏
	
	double OpenProfit_short;//空单浮动盈亏

	double margin;//持仓占用保证金

private:
	int m_tradeID;

	cDate m_openDate;

	string m_instrumentID;//合约代码

	double m_lastPrice;//最新价，时刻保存合约的最新价，平仓用

	double m_PreSettlementPrice;//上次结算价，对隔夜仓有时候要用，快期有用
	
	int m_holding_long;//多单持仓量
	
	int m_holding_short;//空单持仓量

	int m_TodayPosition_long;//多单今日持仓
	
	int m_YdPosition_long;//多单上日持仓

	int m_TodayPosition_short;//空单今日持仓
	
	int m_YdPosition_short;//空单上日持仓

};

typedef shared_ptr< cPositionDetail > cPositionDetailPtr;


#endif
