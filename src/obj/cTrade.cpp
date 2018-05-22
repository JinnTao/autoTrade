#include <cTrade.h>
#include <cOrder.h>

cTrade::cTrade()
:m_tradeID( -1 )
,m_orderID( -1 )
,m_instrumentID( "" )
,m_accountID( "" )
,m_direction( '0' )
,m_offsetFlag( '0' )
,m_price( 0.0 )
,m_volume( 0 )
,m_tradeTime( "" )
,m_tradeDate("")
{
}

cTrade::cTrade( CThostFtdcTradeField* pTrade )
{
    m_tradeID = atoi( pTrade->TradeID );
    m_orderID = atoi( pTrade->OrderSysID );
    m_instrumentID = string( pTrade->InstrumentID );
    m_accountID = string( pTrade->InvestorID );
    m_direction = pTrade->Direction;
    m_offsetFlag = pTrade->OffsetFlag;
    m_price = pTrade->Price;
    m_volume = pTrade->Volume;
    m_tradeDate = string( pTrade->TradeDate);
    m_tradeTime = string( pTrade->TradeTime );
    m_commission = 0;
    m_exchange = string (pTrade->ExchangeID);
}


//cTrade::cTrade( cOrder* pOrder, int id )
//{
//    m_tradeID = id;
//    m_orderID = pOrder->GetOrderID();
//    m_instrumentID = pOrder->GetInstrumentID();
//    m_accountID = "demo";
//    m_direction = pOrder->GetDirection();
//    m_offsetFlag = pOrder->GetOffsetFlag();
//    m_price = pOrder->GetPrice();
//    m_volume = pOrder->GetVolumeTraded();
//    m_tradeTime = pOrder->GetOrderServerTime();
//}

cTrade::cTrade( const cTrade& in )
{
    yr_assert( this != & in );
    m_tradeID = in.m_tradeID;
    m_orderID = in.m_orderID;
    m_instrumentID = in.m_instrumentID;
    m_accountID = in.m_accountID;
    m_direction = in.m_direction;
    m_offsetFlag = in.m_offsetFlag;
    m_price = in.m_price;
    m_volume = in.m_volume;
    m_tradeTime = in.m_tradeTime;
    m_tradeDate = in.m_tradeDate;
    m_commission = in.m_commission;
    m_exchange = in.m_exchange;
}

cTrade& cTrade::operator = ( const cTrade& in )
{
    if( &in != this )
    {
        m_tradeID = in.m_tradeID;
        m_orderID = in.m_orderID;
        m_instrumentID = in.m_instrumentID;
        m_accountID = in.m_accountID;
        m_direction = in.m_direction;
        m_offsetFlag = in.m_offsetFlag;
        m_price = in.m_price;
        m_volume = in.m_volume;
        m_tradeTime = in.m_tradeTime;
        m_tradeDate = in.m_tradeDate;
        m_exchange = in.m_exchange;
    }
    return *this;
}

void cTrade::Print() const
{
    printf( "TradeID:%d ", m_tradeID );
    cerr << "InstrumentID:  " << m_instrumentID ;
    printf( " Direction:%s ", m_direction == '0' ? "B" : "S" );
    printf( "Volume:%d;", m_volume );
    printf( "Price:%5.3f ", m_price );
    cout << "TradeTime " << m_tradeDate << " " <<m_tradeTime;
    cout << "commission " << m_commission ;
    switch( m_offsetFlag )
    {
    case '0':
        printf( " Type:Open" );
        break;
    case '1':
        printf( " Type:Close" );
        break;
    case '3':
        printf( " Type:CloseToday" );
        break;
    case '4':
        printf( " Type:CloseYesterday" );
        break;
    default:
        break;
    }
    printf( "\n" );
}

void cTrade::setCommission(CThostFtdcInstrumentCommissionRateField*pCom,CThostFtdcInstrumentField *pInstField){
    if(pCom != NULL){
        //上期所单独处理
        if(m_exchange == "SHFE"){
            if(m_offsetFlag == THOST_FTDC_OF_Open){
                m_commission = m_volume * pCom->OpenRatioByVolume + m_price * m_volume * pCom->OpenRatioByMoney * pInstField->VolumeMultiple;
            }
            if(m_offsetFlag == THOST_FTDC_OF_CloseToday){
                m_commission = m_volume * pCom->CloseTodayRatioByVolume + m_price * m_volume * pCom->CloseTodayRatioByMoney* pInstField->VolumeMultiple;
            }
            if(m_offsetFlag == THOST_FTDC_OF_CloseYesterday){
                m_commission = m_volume * pCom->CloseRatioByVolume + m_price * m_volume * pCom->CloseRatioByMoney* pInstField->VolumeMultiple;
            }
        }else{
            if(m_offsetFlag == THOST_FTDC_OF_Open){
                m_commission = m_volume * pCom->OpenRatioByVolume + m_price * m_volume * pCom->OpenRatioByMoney* pInstField->VolumeMultiple;
            }
            if(m_offsetFlag == THOST_FTDC_OF_Close){
                m_commission = m_volume * pCom->CloseRatioByVolume + m_price * m_volume * pCom->CloseRatioByMoney* pInstField->VolumeMultiple;
            }
        
        }
    }
}