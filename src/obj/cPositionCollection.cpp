#include <cPositionCollection.h>
#include <cVector.h>
#include <cTrade.h>
#include <easylogging++.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif
cPositionCollection::cPositionCollection()
{
    m_positionMap.clear();
}

cPositionCollection::~cPositionCollection()
{

}

void cPositionCollection::PrintDetail() 
{
    double closeProfit = 0,openProfit = 0,margin = 0;
        for(map<string, cPositionDetailPtr>::iterator iter = m_positionMap.begin(); iter != m_positionMap.end(); iter++)
        {
            //if(iter->second->getHoldLong() != 0 || iter->second->getHoldShort() != 0){
                iter->second->Print();         

                //账户平仓盈亏
                closeProfit = closeProfit + iter->second->closeProfit_long + iter->second->closeProfit_short;
       
                //账户浮动盈亏
                openProfit = openProfit + iter->second->OpenProfit_long + iter->second->OpenProfit_short;

                margin += iter->second->margin;
             //}
        }
        LOG(INFO) << "Total profit: " << closeProfit + openProfit << " OpenProfit: " << openProfit << " CloseProfit: " << closeProfit
        << " Margin: " << margin;
}
void cPositionCollection::update(CThostFtdcInvestorPositionField* pInvestorPosition){
        bool find_trade_message_map = false;
        // filtration positon is zero 
        //if(pInvestorPosition->Position != 0){
        //    
            for(map<string, cPositionDetailPtr>::iterator iter = m_positionMap.begin(); iter!= m_positionMap.end();iter++)
            {
                if(strcmp( (iter->first).c_str(), pInvestorPosition->InstrumentID)==0 && pInvestorPosition->PosiDirection == iter->second->)//合约已存在
                {
                    find_trade_message_map = true;
                    break;
                }
            }
            // create new postion Detail
            if(!find_trade_message_map )
            {

                cPositionDetailPtr trade_message_p = make_shared<cPositionDetail>(pInvestorPosition->InstrumentID);

                m_positionMap.insert(pair<string, cPositionDetailPtr> (pInvestorPosition->InstrumentID, trade_message_p));
            }
            //update position
            m_positionMap[pInvestorPosition->InstrumentID]->update(pInvestorPosition);
        //}
}
void cPositionCollection::update(CThostFtdcTradeField* pTrade){
        bool find_trade_message_map = false;
        // new open 
        if(pTrade->OffsetFlag == THOST_FTDC_OF_Open){
            
            for(map<string, cPositionDetailPtr>::iterator iter = m_positionMap.begin(); iter!= m_positionMap.end();iter++){
                if(strcmp( (iter->first).c_str(), pTrade->InstrumentID)==0)//合约已存在
                {
                    find_trade_message_map = true;
                    break;
                }
            }
            // create new postion Detail
            if(!find_trade_message_map ){

                cPositionDetailPtr trade_message_p  = make_shared<cPositionDetail>(pTrade->InstrumentID);

                m_positionMap.insert(pair<string, cPositionDetailPtr> (pTrade->InstrumentID, trade_message_p));
            }
            
        }
        //update position
        m_positionMap[pTrade->InstrumentID]->update(pTrade);
}



int cPositionCollection::getHolding_long(string instID)
{

    if(m_positionMap.size()==0 || m_positionMap.count(instID) == 0)
        return 0;
    else
        return m_positionMap[instID]->getHoldLong();
}



int cPositionCollection::getHolding_short(string instID)
{

    if(m_positionMap.size()==0 ||  m_positionMap.count(instID) == 0)
        return 0;
    else
        return m_positionMap[instID]->getHoldShort();
}
