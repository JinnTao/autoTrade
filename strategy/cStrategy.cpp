#include <cStrategy.h>



cStrategy::cStrategy()
{
	m_pTradingThread = new cThread< cStrategy >( this, &cStrategy::AutoTrading );
	m_timeSpan = 500;
}

cStrategy::cStrategy( const string& strategyID )
{
	m_pTradingThread = new cThread< cStrategy >( this, &cStrategy::AutoTrading );
	m_timeSpan = 500;
	this->m_strategyName = strategyID;
}

cStrategy::~cStrategy( )
{
}

void cStrategy::start(){
	this->m_status = true;
	m_pTradingThread->Init();
}

void cStrategy::stop(){
	this->m_status = false;
}


DWORD cStrategy::AutoTrading(){
	init();
	while(this->m_status){
		this->run();
		Sleep(this->m_timeSpan);

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