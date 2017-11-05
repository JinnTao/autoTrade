#include <cTradeCollection.h>
#include <direct.h>
cTradeCollection::cTradeCollection()
{
	_it = _map_trade.end();
}

cTradeCollection::~cTradeCollection()
{
	Clear();
}

void cTradeCollection::Clear()
{
	_map_trade.clear();
	_m_trade_instrument.clear();
	_m_trade_order.clear();
	_m_trade_handle.clear();
}

void cTradeCollection::Add( CThostFtdcTradeField* pTrade,CThostFtdcInstrumentCommissionRateField *pCom ,CThostFtdcInstrumentField *pInstFiled)
{
	if( pTrade->Volume > 0 )
	{
		shared_ptr< cTrade > ptr = make_shared< cTrade >( pTrade );
		AddToMapInternal( ptr ,pCom,pInstFiled);
	}
}

void cTradeCollection::Add( cTradePtr p_element ,CThostFtdcInstrumentCommissionRateField*pCom,CThostFtdcInstrumentField* pInstField)
{
	AddToMapInternal( p_element,pCom ,pInstField); 
}

int cTradeCollection::Count() const
{
	int count = 0;
	for( mapType::const_iterator it = _map_trade.begin(); it != _map_trade.end(); ++it )
		++count;
	return count;
}

void cTradeCollection::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	for( tradeStoreByInstrument::const_iterator it = _m_trade_instrument.begin(); it != _m_trade_instrument.end(); ++it )
		instrumentIDs.push_back( (*it).first );
}

void cTradeCollection::GetOrderIDs( cIvector& orderIDs ) const
{
	for( tradeStoreByOrder::const_iterator it = _m_trade_order.begin(); it != _m_trade_order.end(); ++it )
		orderIDs.push_back( (*it).first );
}

void cTradeCollection::GetTradeIDs( cIvector& tradeIDs ) const
{
	for( mapType::const_iterator it = _map_trade.begin(); it != _map_trade.end(); ++it )
		tradeIDs.push_back( (*it).first );
}

cTrade* cTradeCollection::GetTradeHandle( int tradeID )
{
	cTradePtr p = GetTradeHandleSharedPtr( tradeID );
	if( p )
		return p.get();
	else
		return NULL;
}

cTradePtr cTradeCollection::GetTradeHandleSharedPtr( int tradeID )
{
	tradeHandle::iterator it = _m_trade_handle.find( tradeID );
	if( it != _m_trade_handle.end() )
		return (*it).second;
	else
		return cTradePtr();
}

cArray< const cTrade* > cTradeCollection::GetTradeByInstrument( const cString& instrumentID ) const
{
	tradeStoreByInstrument::const_iterator it = _m_trade_instrument.find( instrumentID );
	if( it != _m_trade_instrument.end() )
		return (*it).second;
	else
		return cArray< const cTrade* >();
}

cArray< const cTrade* > cTradeCollection::GetTradeByOrder( int orderID ) const
{
	tradeStoreByOrder::const_iterator it = _m_trade_order.find( orderID );
	if( it != _m_trade_order.end() )
		return (*it).second;
	else
		return cArray< const cTrade* >();
}

void cTradeCollection::AddToMapInternal( shared_ptr< cTrade >& element,CThostFtdcInstrumentCommissionRateField* pCom,CThostFtdcInstrumentField *pInstField )
{
	//
	int tradeID = element->GetTradeID();
	//
	element->setCommission(pCom,pInstField);
	tradeHandle::iterator it = _m_trade_handle.find( tradeID );
	if( it == _m_trade_handle.end() )
		_m_trade_handle.insert( tradeHandle::value_type( tradeID, element ) );
	else
		(*it).second = element;
}

void cTradeCollection::PrintAll() const
{
	tradeHandle::const_iterator it;
	if (_m_trade_handle.size() == 0){
		cerr << "  No trade List" << endl;
		return ;
	}
	_mkdir("output//tradeList");
	ofstream output;
	string s_userId =  (*_m_trade_handle.begin()).second->GetAccountID();
	string date =  (*_m_trade_handle.begin()).second->GetTradeDate();
	output.open("output/tradeList/" + s_userId  + "_" + date + ".csv", ios::ate) ;
	output <<"合约"<<","<< "日期"<< "," << "时间"<< "," << "方向"<< "," << "标志"<< "," << "成交笔数"<< ","<< "成交价格"<< ","<< "手续费"<< endl;
	for( it = _m_trade_handle.begin(); it != _m_trade_handle.end(); ++it ){
		(*it).second->Print();
		output
		<<(*it).second->GetInstrumentID()<< "," 
		<< (*it).second->GetTradeDate()<< ","
		<< (*it).second->GetTradeTime() <<","
		<< ((*it).second->GetDirection()=='0'?"买":"卖" )<< ","
		<<((*it).second->GetOffsetFlag()=='0'?"开仓":"平仓" )<< ","
		<<  (*it).second->GetVolume() << ","
		<< (*it).second->GetPrice()<<","
		<< (*it).second->GetCommission()
		<<endl;

	}
	output.close();
}

void cTradeCollection::PrintTrade( int tradeID )
{
	cTrade* p = GetTradeHandle( tradeID );
	if( p )
		p->Print();
}

