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

    void PrintDetail() ;
    /*update position*/
    void update( CThostFtdcInvestorPositionField* pInvestorPositionDetail);

    void update( CThostFtdcTradeField* pTrade);

    int getPosition(string instID,int dire);
    
    int getYdPosition(string instID, int dire);

    int getTdPosition(string instID, int dire);
    
    bool postionDireEqual();

protected:
    //用最简单的方式实现，如果太多的tyedef 是不是结构过于复杂，还是有新的想法？
    //mapType _map_position;
    //mapType::iterator _it;
    //positionStore _m_position_instrument;
    //positionHandle _m_position_tradeid;
    
    std::list<cPositionDetailPtr>  position_list_;
private:

};

typedef shared_ptr< cPositionCollection > cPositionCollectionPtr;


#endif
