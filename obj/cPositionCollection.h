#ifndef __CPOSITIONCOLLECTION_H__
#define __CPOSITIONCOLLECTION_H__

#include <cPosition.h>
#include <map>

//template< class T > class cArray;
//
//// instrument string versus array of sPositionDetail, i.e. IF1603 vs. 2 different positions
//typedef map< cString, cArray< const cPositionDetail* > > positionStore;
//// trade ID versus cMarketDataPtr, i.e. 44060
//typedef map< int, cPositionDetailPtr > positionHandle;


class cPositionCollection
{
public:
	cPositionCollection();
	virtual ~cPositionCollection();

	//void Clear();
	//int Count() const;

	////
	///*Add new position*/
	//void Add( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail );
	//void Add( CThostFtdcTradeField* pTrade );
	//void Add( cPositionDetailPtr p_element );
	//void Add( cTrade* pTrade );
	////
	///*Remove position*/
	//void Remove( CThostFtdcTradeField* pTrade );
	//void Remove( cTrade* pTrade );
	//void Remove( int );
	//void RemoveTradesList( CThostFtdcTradeField* pTrade, cIvector& tradeIDsToRemove );
	//void RemoveTradesList( cTrade* pTrade, cIvector& tradeIDsToRemove );
	////
	///*Get methods*/
	//cPositionDetail* GetPositionDetail( int );
	//cPositionDetailPtr GetPositionDetailSharedPtr( int );
	//cArray< const cPositionDetail* > GetPositionDetailByInstrument( const cString& ) const;

	//void SummaryByInstrument() const;
	//void SummaryByInstrument( const cString&, int&, int&, int&, int& ) const;
	void PrintDetail() ;
	/*update position*/
	void update( CThostFtdcInvestorPositionField* pInvestorPositionDetail);

	void update( CThostFtdcTradeField* pTrade);

	int getHolding_long(string instID);
	int getHolding_short(string instID);

	int getYdLong(string instID){return this->m_positionMap[instID]->getYdLong();}
	int getTdLong(string instID){return this->m_positionMap[instID]->getTdLong();}

	int getYdShort(string instID){return this->m_positionMap[instID]->getYdShort();}
	int getTdShort(string instID){return this->m_positionMap[instID]->getTdShort();}
protected:
	//用最简单的方式实现，如果太多的tyedef 是不是结构过于复杂，还是有新的想法？
	//mapType _map_position;
	//mapType::iterator _it;
	//positionStore _m_position_instrument;
	//positionHandle _m_position_tradeid;
	map<string,cPositionDetailPtr> m_positionMap;//持仓记录 
private:
	//void AddToMapInternal( shared_ptr< cPositionDetail >& element );
};

typedef shared_ptr< cPositionCollection > cPositionCollectionPtr;


#endif
