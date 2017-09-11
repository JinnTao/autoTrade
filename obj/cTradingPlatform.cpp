//
#include <cSystem.h>
#include <cTradingPlatform.h>
#include <cTraderSpi.h>
#include <cTick.h>
#include <FuturesCNUtils.h>
#include <iostream>
#include <string>
using std::string;
#ifndef _DEBUG
#define _DEBUG 0
#endif
bool CompareStringArray( const cArray< cString >& strArray1, const cArray< cString >& strArray2 )
{
	if( &strArray1 == &strArray2 )
		return true;
	if( strArray1.getSize() != strArray2.getSize() )
		return false;

	for( int i = 0; i < strArray1.getSize(); ++i )
	{
		cString str1 = strArray1[i];
		int j;
		for( j = 0; j < strArray2.getSize(); ++j )
		{
			
			if( Compare( str1, strArray2[j] ) )
				break;
		}
		if( j >= strArray2.getSize() )
			return false;
	}
	return true;
}

cTradingPlatform::cTradingPlatform()
: m_pTraderSpi( NULL )
, m_nRequestID( 0 )
, m_runAutoTrade( true )
{
	if( m_pMarketDataEngine.get() )
		m_pMarketDataEngine.reset();

	if( m_pStrategy.get() )
		m_pStrategy.reset();

	m_pPositions = make_shared< cPositionCollection >();
	m_pOrders = make_shared< cOrderCollection >();
	m_pTrades = make_shared< cTradeCollection >();
	m_pSubscribeInst = make_shared<vector<string>>();
	m_pInstMessageMap = new map<string, CThostFtdcInstrumentField*>();

//	m_pInstMessageMap = make_shared<map<string,CThostFtdcInstrumentField*>>();
}

cTradingPlatform::~cTradingPlatform()
{
	ClearPlatform();
}

void cTradingPlatform::RegisterTraderSpi( cTraderSpi* pTraderSpi )
{
	if( m_pTraderSpi == pTraderSpi )
		return;

	m_pTraderSpi = pTraderSpi;
	
	m_pTraderSpi->RegisterPositionCollection( m_pPositions );
	m_pTraderSpi->RegisterOrderCollection( m_pOrders );
	m_pTraderSpi->RegisterTradeCollection( m_pTrades );
	m_pTraderSpi->RegisterInstMessageMap(m_pInstMessageMap);
	m_pTraderSpi->RegisterSubscribeInstList(m_pSubscribeInst);
	//cArray< cString > instrumentIDs;
	//pTraderSpi->GetInstrumentIDs( instrumentIDs );

	//if( !m_instrumentIDs.getSize() )
	//{		
	//	for( int i = 0; i < instrumentIDs.getSize(); ++i )
	//		m_instrumentIDs.push_back( instrumentIDs[i] );
	//}
	//else
	//{
	//	if( !CompareStringArray( m_instrumentIDs, instrumentIDs ) )
	//		yr_error( "cTraderSpi and cTradingPlatform has different instruments" );
	//}

	//for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	//{
	//	map< cString, double >::iterator it = m_closedPnL.find( m_instrumentIDs[i] );
	//	if( it == m_closedPnL.end() )
	//	{
	//		m_closedPnL.insert( map< cString, double >::value_type( m_instrumentIDs[i], 0.0 ) );
	//	}
	//}
}


void cTradingPlatform::RegisterMdSpi( cMdSpi* p )
{
	if( m_pMdSpi == p )
		return;

	m_pMdSpi = p;
}

void cTradingPlatform::RegisterMarketDataEngine( cMarketDataCollectionPtr pMarketDataEngine )
{
	if( m_pMarketDataEngine.get() )
		m_pMarketDataEngine.reset();

	m_pMarketDataEngine = pMarketDataEngine;
/*
	cArray< cString > instrumentIDs;
	pMarketDataEngine->GetInstrumentIDs( instrumentIDs );

	if( !m_instrumentIDs.getSize() )
	{	
		
		for( int i = 0; i < instrumentIDs.getSize(); ++i )
			m_instrumentIDs.push_back( instrumentIDs[i] );
	}
	else
	{
		if( !CompareStringArray( m_instrumentIDs, instrumentIDs ) )
			yr_error( "cMarketDataCollection and cTradingPlatform has different instruments" );
	}

	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	{
		map< cString, double >::iterator it = m_closedPnL.find( m_instrumentIDs[i] );
		if( it == m_closedPnL.end() )
		{
			m_closedPnL.insert( map< cString, double >::value_type( m_instrumentIDs[i], 0.0 ) );
		}
	}
*/
}

void cTradingPlatform::RegisterStrategy( cStrategyPtr pStrategy )
{

}



void cTradingPlatform::SendNewOrder( cOrder* pOrder )
{
	
}

void cTradingPlatform::PrintClosePnL( int tradeID ) const
{


}


const sInstrumentInfo* cTradingPlatform::GetInstrumentInfo( const cString& instrumentID ) const
{
	if( m_pTraderSpi )
		return m_pTraderSpi->GetInstrumentInfo( instrumentID );
	else
	{
		return NULL;
		// demo paper trading
		//@todo
	}
}

const sTradingAccountInfo* cTradingPlatform::GetTradingAccountInfo() const
{
	if( m_pTraderSpi )
		return m_pTraderSpi->GetTradingAccountInfo();
	else
	{
		return NULL;
		// demo paper trading
		//@todo

	}
}

void cTradingPlatform::CancelPendingOrders()
{

}

void cTradingPlatform::CancelPendingOrders( const cString& instrumentID )
{

}

void cTradingPlatform::CancelPendingOrder( int orderID )
{
	//cOrder* pOrder = m_pOrders->GetOrderHandle( orderID );
	//if( m_pTraderSpi )
	//{
	//	if( pOrder && pOrder->IsPendingOrder() )
	//		m_pTraderSpi->ReqOrderAction( pOrder );
	//}
	//else
	//{
	//	//
	//	/*
	//		here we do the demo trade and no TraderSpi is required
	//		and thus we simply change m_orders
	//	*/
	//	m_pOrders->Remove( orderID );
	//}
}

DWORD cTradingPlatform::AutoTrading()
{

	string str;
	char dire[50],offset[50],inst[50],price[50],order[50],tag[50];
	int vol, mark = 0,orderNo;
	cerr<<"--------------------Human-computer interaction function Start--------------------------------"<<endl;
	cerr<<endl<<"OrderList: help | show | order| trade | stop | run |close |buy/sell open/close inst vol price| cancle seqNo£º";
	
	//initial subcribe instrument
	this->m_pMdSpi->SubscribeMarketData(this->m_pSubscribeInst);
	while(true)
	{
		//std::cin>>str;
		memset(price,0,50);
		memset(order,0,50);
		memset(tag,0,50);
		vol = 0;
		orderNo = 0;
		getline(std::cin,str);
		if(str == "show"){
			m_pTraderSpi->showPositionDetail();
		}

		else if(str == "close")
		{	
			cerr<<"close Position:"<<endl;
			//g_pUserSpi_tradeAll->ForceClose();
		}
		else if(str == "run")
		{					
			//g_strategy->set_allow_open(true);
		}
		else if(str == "stop")
		{
			cerr<<"stop:"<<endl;
			//g_strategy->set_allow_open(false);
		}
		else if(str == "order"){
			this->m_pOrders->PrintPendingOrders();
		}
		else if(str == "trade"){
			this->m_pTrades->PrintAll();
		}else if(str == "help"){
			cerr<<"OrderList: show | order| trade | stop | run |close |buy/sell open/close inst vol price -> ";
		}
		else if(str == "account"){
			this->m_pTraderSpi->ReqQryTradingAccount();
		}
		else if(str.length() >7)
		{
			// insert order
			sscanf(str.c_str(),"%s %s %s %d %s",dire,offset,inst,&vol,price);
			double dPrice = atof(price);
			if(vol!=0){
				this->insertOrder(string(inst),string(dire),string(offset),vol,dPrice);
			}
			// cancle order
			sscanf(str.c_str(),"%s %d",order,&orderNo);
			if(orderNo !=0 ){
				this->cancleOrder(order,orderNo);
			}
			

		}
	}
	return 0;
}


DWORD cTradingPlatform::ProcessOrderTest()
{
	return 0;
}

DWORD cTradingPlatform::IOProcess()
{
//	return m_pMarketDataEngine->IOProcess();
	return 0;
}

DWORD cTradingPlatform::SimulationIOProcess()
{
	//if( m_pMarketDataEngine.get() )
	//	return m_pMarketDataEngine->SimulationIOProcess();
	//else
	//	return 0;
	return 0;
}


void cTradingPlatform::PrintPendingOrders() const
{
	m_pOrders->PrintPendingOrders();
}

void cTradingPlatform::PrintCancelledOrders() const
{
	m_pOrders->PrintCancelledOrders();
}

void cTradingPlatform::PrintAllOrders() const
{
	m_pOrders->PrintAllOrders();
}

void cTradingPlatform::PrintPositionSummary() const
{
//	m_pPositions->SummaryByInstrument();
}



bool cTradingPlatform::SimulationUpdate( const cTick& tick )
{
	//if( m_pTraderSpi )
	//	return false;

	//m_pMarketDataEngine->SimulationUpdate( tick );
	//bool flag = m_pStrategy->SimTimeIndicatorSignalUpdate( tick );
	//return flag;
	return true;
}

void cTradingPlatform::Sleep()
{
	int hh = cSystem::HourNow();
	int mm = cSystem::MinuteNow();
	bool flag = false;
	if( hh > 2 && hh < 8 )
		flag = true;
	else if( hh == 8 && mm < 30 )
		flag = true;

	if( flag )
	{
		//
		if( m_pTrades->Count() )
			m_pTrades->Clear();
		//
		if( m_pOrders->Count() )
			m_pOrders->Clear();

	}
}

void cTradingPlatform::WakeUp()
{

}


void cTradingPlatform::ClearPlatform()
{
//	/*m_instrumentIDs.clear();*/
	m_runAutoTrade = false;
	m_nRequestID = 0;
	m_pTraderSpi = NULL;

	/*if( m_pMarketDataEngine.get() )
		m_pMarketDataEngine.reset();

	if( m_pStrategy.get() )
		m_pStrategy.reset();*/

	if( m_pPositions.get() )
	{
//		m_pPositions->Clear();
		m_pPositions.reset();
	}

	if( m_pOrders.get() )
	{
		m_pOrders->Clear();
		m_pOrders.reset();
	}

	if( m_pTrades.get() )
	{
		m_pTrades->Clear();
		m_pTrades.reset();
	}

}
void cTradingPlatform::cancleOrder(string order,int seqNo){
	if(order == "cancle"){
		
		shared_ptr<cOrder> pOrder = NULL;
		if(seqNo != -1){
			if(!this->m_pOrders->getOrderByNo(seqNo,pOrder)){
				cerr<<"  Order Seq No Exist."<<endl;
				return;
			}
			this->m_pTraderSpi->ReqOrderAction(pOrder);
		}else{
			vector<cOrderPtr> vOrder = this->m_pOrders->GetAllOrder();
			for(auto it = vOrder.begin();it!=vOrder.end();it++){
				if(it->get()->IsPendingOrder()){
					this->m_pTraderSpi->ReqOrderAction(*it);
				}
			}
		}
	}
}
void cTradingPlatform::cancleAllOrder(string order,string tag){
	//if(order == "cancle" && tag == "all"){
	//	vector<cOrderPtr> vOrder = this->m_pOrders->GetAllOrder();
	//	for(auto it = vOrder.begin();it!=vOrder.end();it++){

	//		this->m_pTraderSpi->ReqOrderAction(it->get()->GetOrderID());
	//	}
	//}
}

void cTradingPlatform::insertOrder(string inst,string dire,string flag, int vol,double orderPrice){
	// get parameters type
	DIRECTION eDire;
	OFFSETFLAG eFlag;
	if(dire == "buy"){
		eDire = DIRECTION::buy;
	}
	else if(dire == "sell"){
		eDire =	DIRECTION::sell;
	}else{
		cerr << "input parameter Error" << endl;
		return;
	}
	
	if(flag == "open"){
		eFlag = OFFSETFLAG::open;
	}
	else if(flag == "close"){
		eFlag = OFFSETFLAG::close;;
	}else{
		cerr << "input parameter Error" << endl;
		return;
	}

	// make market price order
	if(orderPrice == 0){
		cMarketData *p;
		double lastprice = 0;
		p = this->m_pMarketDataEngine->GetMarketDataHandle(inst);
		if(p) {
			 lastprice = p->getLastMarketData().LastPrice;
		}
		switch (eDire)
		{
		case buy:
			orderPrice = lastprice + this->m_pInstMessageMap->at(inst)->PriceTick;
			break;
		case sell:
			orderPrice =  lastprice - this->m_pInstMessageMap->at(inst)->PriceTick;
			break;
		}
	}
	
	// go into order
	this->m_pTraderSpi->insertOrder(inst,eDire,eFlag,vol,orderPrice);
	_sleep(500);// wait 500ms for pTrader response.
}