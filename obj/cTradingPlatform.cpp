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
	m_pSignals = make_shared< cSignalCollection >();

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
	if( m_pStrategy.get() )
		m_pStrategy.reset();

	m_pStrategy = pStrategy;

	m_pStrategy->RegisterSignalCollection( m_pSignals );

	cArray< cString > instrumentIDs;
	pStrategy->GetInstrumentIDs( instrumentIDs );

	if( !m_instrumentIDs.getSize() )
	{	
		for( int i = 0; i < instrumentIDs.getSize(); ++i )
			m_instrumentIDs.push_back( instrumentIDs[i] );
	}
	else
	{
		if( !CompareStringArray( m_instrumentIDs, instrumentIDs ) )
			yr_error( "cStrategy and cTradingPlatform has different instruments" );
	}

	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	{
		map< cString, double >::iterator it = m_closedPnL.find( m_instrumentIDs[i] );
		if( it == m_closedPnL.end() )
		{
			m_closedPnL.insert( map< cString, double >::value_type( m_instrumentIDs[i], 0.0 ) );
		}
	}


}

void cTradingPlatform::SendNewOrders( const cString& instrumentID )
{
	cArray< cOrder* > orders = m_pStrategy->GenNewOrders( instrumentID, m_pPositions.get() );
	for( int i = 0; i < orders.getSize(); ++i )
	{
		SendNewOrder( orders[i] );
	}
}

void cTradingPlatform::SendNewOrder( cOrder* pOrder )
{
	//if( m_pTraderSpi )
	//	m_pTraderSpi->ReqOrderInsert( pOrder );
	//else
	//{
	//	//demo trade and no traderspi is required
	//	//also assume all orders are fully executed with no pending orders
	//	shared_ptr< cOrder > p = make_shared< cOrder >();
	//	++m_nRequestID;
	//	p->SetOrderID( m_nRequestID );
	//	p->SetInstrumentID( pOrder->GetInstrumentID() );
	//	if( pOrder->GetDirection() == '0' )
	//		p->SetDirection( 0 );
	//	else
	//		p->SetDirection( 1 );

	//	if( pOrder->GetOffsetFlag() == '0' )
	//		p->SetOffsetFlag( 0 );
	//	else if( pOrder->GetOffsetFlag() == '1' )
	//		p->SetOffsetFlag( 1 );
	//	else if( pOrder->GetOffsetFlag() == '3' )
	//		p->SetOffsetFlag( 3 );
	//	else if( pOrder->GetOffsetFlag() == '4' )
	//		p->SetOffsetFlag( 4 );

	//	p->SetVolumeOriginal( pOrder->GetVolumeOriginal() );
	//	p->SetVolumeTraded( pOrder->GetVolumeTraded() );
	//	p->SetPrice( pOrder->GetPrice() );
	//	p->SetOrderTime( pOrder->GetOrderServerTime() );

	//	if( _DEBUG )
	//	{
	//		cout << p->GetOrderID() << ":" << p->GetOrderServerTime().TimeString().c_str() << ": ";
	//		cout << p->GetInstrumentID() << " " << p->GetDirection() << " " << p->GetOffsetFlag() << " " << p->GetPrice() << endl;
	//		PrintClosePnL();
	//	}

	//	
	//	//1. add order into collection
	//	m_pOrders->Add( p );
	//	//3. create a trade from the order
	//	//   assume the orderID and the trade ID are the same
	//	cTradePtr pTrade = make_shared< cTrade >( p.get(), p->GetOrderID() );
	//	//4. add trade into collection
	//	m_pTrades->Add( pTrade );
	//	//5. update positions
	//	if( pTrade->GetOffsetFlag() == '0' )
	//	{
	//		// add new positions
	//		m_pPositions->Add( pTrade.get() );
	//	}
	//	else
	//	{
	//		// unwind existing positions
	//		m_pPositions->Remove( pTrade.get() );
	//		/* new code */
	//		cIvector tradeIDsToRemove;
	//		m_pPositions->RemoveTradesList( pTrade.get(), tradeIDsToRemove );

	//	}
	//	
	//	delete pOrder;
	//}
}

void cTradingPlatform::PrintClosePnL( int tradeID ) const
{
	//cPositionDetail* pPosition = m_pPositions->GetPositionDetail( tradeID );
	//double openPrice = pPosition->GetPrice();
	//double volume = pPosition->GetVolume();


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
	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
		CancelPendingOrders( m_instrumentIDs[i] );
}

void cTradingPlatform::CancelPendingOrders( const cString& instrumentID )
{
	cIvector orderIDs;
	m_pStrategy->CancelPendingOrders( instrumentID, m_pOrders.get(), orderIDs );
	for( int i = 0; i < orderIDs.getSize(); ++i )
		CancelPendingOrder( orderIDs[i] );

}

void cTradingPlatform::CancelPendingOrder( int orderID )
{
	cOrder* pOrder = m_pOrders->GetOrderHandle( orderID );
	if( m_pTraderSpi )
	{
		if( pOrder && pOrder->IsPendingOrder() )
			m_pTraderSpi->ReqOrderAction( pOrder );
	}
	else
	{
		//
		/*
			here we do the demo trade and no TraderSpi is required
			and thus we simply change m_orders
		*/
		m_pOrders->Remove( orderID );
	}
}

DWORD cTradingPlatform::AutoTrading()
{

	string str;
	char dire[50];
	char offset[50];
	char inst[50];
	int vol;
	char price[50];
	int mark = 0;
	int a;
	cerr<<"--------------------Human-computer interaction function Start--------------------------------"<<endl;
	cerr<<endl<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º";

	while(1)
	{
		//std::cin>>str;
		getline(cin,str);
		if(str == "show")
			a = 0;	
		else if(str == "close")
			a = 1;
		else if(str == "run")
			a = 2;
		else if(str == "stop")
			a = 3;
		else if(str.length() >10){
				sscanf(str.c_str(),"%s %s %s %d %s",dire,offset,inst,&vol,price);
				double dPrice = atof(price);
				this->insertOrder(string(inst),string(dire),string(offset),vol,dPrice);
				a = 4;
			}
		else if(str == "order"){
			a = 5;
		}
		else if(str == "trade"){
			a = 6;
		}
		switch(a){
		case 0:
			{		
				cerr<<"show Postion:"<<endl;
				//g_pUserSpi_tradeAll->printTrade_message_map("");
				m_pTraderSpi->showPositionDetail();
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;
			}
		case 1:
			{
				cerr<<"close Position:"<<endl;
				//g_pUserSpi_tradeAll->ForceClose();
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;
			}
		case 2:
			{
				cerr<<"run:"<<endl;
				//g_strategy->set_allow_open(true);
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;

			}
		case 3:
			{
				cerr<<"stop:"<<endl;
				//g_strategy->set_allow_open(false);
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;

			}
		case 4:
			{
				cerr<<endl<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;
			}
		case 5:
			{
				this->m_pOrders->PrintPendingOrders();
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;
			}
		case 6:
			{
				this->m_pTrades->PrintAll();
				cerr<<"Input Order(show : show ,close : close all position,Run : run Strategy, stop : stop Strategy)£º"<<endl;
				break;
			}


		}

	}
	//while( m_runAutoTrade )
	//{
	//	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	//	{
	//		bool updateFlag = m_pStrategy.get() ? m_pStrategy->RealTimeIndicatorSignalUpdate( m_instrumentIDs[i] ) : false;
	//		if( updateFlag )
	//		{
	//			CancelPendingOrders( m_instrumentIDs[i] );
	//			SendNewOrders( m_instrumentIDs[i] );
	//		}
	//	}
	//}
	return 0;
}


DWORD cTradingPlatform::ProcessOrderTest()
{
	cSystem::Sleep(3000);
	while( true )
	{
		char runTest[1];
		printf( "process order open test (Y/N)?:" );
		cin >> runTest;
		while( !( Compare( runTest, "Y" ) || Compare( runTest, "N" ) ) )
		{
			printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
			cin >> runTest;
		}
		if( Compare( runTest, "Y" ) )
		{
			int id;
			for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
				cout << m_instrumentIDs[i].c_str() << "(" << i << ") ";
			cout << endl;
			cin >> id;
			while( !( id >=0 && id < m_instrumentIDs.getSize() ) )
			{
				printf( "invalid input, id shall be between 0 and %d, please re-enter:", m_instrumentIDs.getSize()-1 );
				cin >> id;
			}
			cOrder pOrder;
			pOrder.SetInstrumentID( m_instrumentIDs[id] );
			printf( "open(0), close(1), close today(3), close yesterday(4)?:" );
			int offsetFlag;		//'0':open, '1':close
			cin >> offsetFlag;
			while( !( offsetFlag == 0 || offsetFlag == 1 || offsetFlag == 3 || offsetFlag == 4 ) )
			{
				printf( "invalid input, either 0 or 1 is allowed. please re-enter:" );
				cin >> offsetFlag;
			}
			pOrder.SetOffsetFlag( offsetFlag );

			//
			int direction;			//'0':buy,'1':short
			printf( "buy(0) or sell(1)?:" );
			cin >> direction;
			while( !( direction == 0 || direction == 1 ) )
			{
				printf( "invalid input, either 0 or 1 is allowed. please re-enter:" );
				cin >> direction;
			}
			pOrder.SetDirection( direction );
			//
			double price;
			unsigned int volume;
			printf( "price:" );
			cin >> price;
			printf( "volume:" );
			cin >> volume;
			pOrder.SetPrice( price );
			pOrder.SetVolumeOriginal( volume );

			SendNewOrder( &pOrder );
			cSystem::Sleep(1000);

		}
		//
		printf( "process order cancelation test (Y/N)?:" );
		cin >> runTest;
		while( !( Compare( runTest, "Y" ) || Compare( runTest, "N" ) ) )
		{
			printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
			cin >> runTest;
		}
		if( Compare( runTest, "Y" ) )
		{
			PrintPendingOrders();
			CancelPendingOrders();
		}
	}
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

void cTradingPlatform::PrintClosePnL() const
{
	double pnl = m_pStrategy->PositionPnL( m_pPositions.get() );
	cout << "position PnL is: " << pnl << endl;
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
		if( m_pSignals->Count() )
			m_pSignals->Clear();
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

	if( m_pSignals.get() )
	{
		m_pSignals->Clear();
		m_pSignals.reset();
	}
}

void cTradingPlatform::insertOrder(string inst,string dire,string flag, int vol,double orderPrice){
	DIRECTION eDire;
	OFFSETFLAG eFlag;
	if(dire == "buy"){
		eDire = DIRECTION::buy;
	}
	if(dire == "sell"){
		eDire =	DIRECTION::sell;
	}
	if(flag == "open"){
		eFlag = OFFSETFLAG::open;
	}
	if(flag == "close"){
		eFlag = OFFSETFLAG::close;;
	}
	//if(orderPrice >)
	if(vol != 0 ){
		
		this->m_pTraderSpi->insertOrder(inst,eDire,eFlag,vol,orderPrice);
	}
	//Sleep(500);
}