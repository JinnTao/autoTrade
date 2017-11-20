#include <cStrategy.h>
#include "easylogging\easylogging++.h"


cStrategy::cStrategy()
{
    //m_thread =  thread(&cStrategy::AutoTrading,this);
	//m_pTradingThread = new cThread< cStrategy >( this, &cStrategy::AutoTrading );
	m_timeSpan = 500;
}

cStrategy::cStrategy( const string& strategyID )
{
    //m_thread = thread(&cStrategy::AutoTrading, this);
	//m_pTradingThread = new cThread< cStrategy >( this, &cStrategy::AutoTrading );
	m_timeSpan = 500;
	this->m_strategyName = strategyID;
}

cStrategy::~cStrategy( )
{
}

void cStrategy::start(){
    //this->m_status = true;
    this->m_isRuning.store(true, std::memory_order_release);
    m_thread = std::thread(&cStrategy::AutoTrading, this);

    //m_thread.join();
	//m_pTradingThread->Init();
}

void cStrategy::stop(){
    m_isRuning.store(false, std::memory_order_release);
    if (m_thread.joinable()) {
        m_thread.join();
    }
	//this->m_status = false;
}


DWORD cStrategy::AutoTrading(){
	init();
	while(this->m_isRuning.load(std::memory_order_relaxed)){
		this->run();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);

	}
	unInit();
	return 0;

}
void cStrategy::init(){
	cerr << this->m_strategyName << " init" << endl;
};

void cStrategy::unInit(){
		cerr << this->m_strategyName << " unInit" << endl;
};

void cStrategy::sendStopOrder(string inst,DIRECTION inDirection,OFFSETFLAG inOffset,double price,UINT volume ,string strategy,int slipNum) {
	cStopOrder order;
	order.instrument = inst;
	order.direction = inDirection;
	order.offset = inOffset;
	order.price = price;
	order.volume = volume;
	order.strategyName = strategy;
	order.orderType = "";
	order.status = true;
	order.slipTickNum = slipNum;
	order.orderTime = std::chrono::system_clock::now();
	// add order to working list
	m_workingStopOrderList.push_back(order);
}

void cStrategy::processStopOrder(string inst, double lastPrice) {
	for(auto var =  m_workingStopOrderList.begin();var!=m_workingStopOrderList.end();var++)
	{
		if (inst == var->instrument && var->status) {
			bool longTriggered = var->direction == DIRECTION::buy && lastPrice >= var->price;
			bool shortTriggered = var->direction == DIRECTION::sell && lastPrice <= var->price;

			if (longTriggered || shortTriggered) {
				//this->m_pTradeSpi->insertOrder(inst, var->direction, var->offset, var->volume, var->price, var->slipTickNum);
				this->m_pTradeSpi->insertOrder(inst, var->direction, var->offset, var->volume, 0, var->slipTickNum);
				var->status = false;

			}
		}

	}
}
bool cStrategy::isTradeTime() {


}
bool cStrategy::mode1() {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500;
    cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = nowDateTime.Hour();
    DateTimeFormat min = nowDateTime.Minute();

    DateTimeFormat nowTime = hour * 100 + min;
    bool newState;
    if ((nowTime >= s0900 && nowTime <s1015) ||
        (nowTime >= s1030 && nowTime <s1130) ||
        (nowTime >= s1330 && nowTime <s1500) 
        )
    {

        newState = true;
    }
    else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode2() {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2300 = 2300;
    cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = nowDateTime.Hour();
    DateTimeFormat min = nowDateTime.Minute();

    DateTimeFormat nowTime = hour * 100 + min;
    bool newState;
    if ((nowTime >= s0900 && nowTime <s1015) ||
        (nowTime >= s1030 && nowTime <s1130) ||
        (nowTime >= s1330 && nowTime <s1500) ||
        (nowTime >= s2100 && nowTime <s2300)
        )
    {

        newState = true;
    }
    else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode3() {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2330 = 2330;
    cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = nowDateTime.Hour();
    DateTimeFormat min = nowDateTime.Minute();

    DateTimeFormat nowTime = hour * 100 + min;
    bool newState;
    if ((nowTime >= s0900 && nowTime <s1015) ||
        (nowTime >= s1030 && nowTime <s1130) ||
        (nowTime >= s1330 && nowTime <s1500) ||
        (nowTime >= s2100 && nowTime <s2330)
        )
    {

        newState = true;
    }
    else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode4() {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2359 = 2359,s0000 = 0,s0100 = 100;
    cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = nowDateTime.Hour();
    DateTimeFormat min = nowDateTime.Minute();

    DateTimeFormat nowTime = hour * 100 + min;
    bool newState;
    if ((nowTime >= s0900 && nowTime <s1015) ||
        (nowTime >= s1030 && nowTime <s1130) ||
        (nowTime >= s1330 && nowTime <s1500) ||
        (nowTime >= s2100 && nowTime <s2359) ||
        (nowTime >= s0000 && nowTime <s0100)
        )
    {

        newState = true;
    }
    else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode5() {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2359 = 2359, s0000 = 0, s0200 = 200;
    cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = nowDateTime.Hour();
    DateTimeFormat min = nowDateTime.Minute();

    DateTimeFormat nowTime = hour * 100 + min;
    bool newState;
    if ((nowTime >= s0900 && nowTime <s1015) ||
        (nowTime >= s1030 && nowTime <s1130) ||
        (nowTime >= s1330 && nowTime <s1500) ||
        (nowTime >= s2100 && nowTime <s2359) ||
        (nowTime >= s0000 && nowTime <s0200)
        )
    {

        newState = true;
    }
    else {

        newState = false;
    }
    return newState;
}