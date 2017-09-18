#include "cStrategyTemplate.h"


cStrategyTemplate::cStrategyTemplate(){
	cStrategy::cStrategy();
}
cStrategyTemplate::cStrategyTemplate(string name){
	cStrategy::cStrategy(name);
}

cStrategyTemplate::~cStrategyTemplate(){

}
void cStrategyTemplate::init(){
	if(m_close.size() == 0){
		this->m_marketData->loadSeriesHistory("rb","2017","2017",DataFrequency::M1,DataType::Open,m_open,m_high,m_low,m_close,m_volume);
	}
	double *m_buffer = new double[m_open.size()];
    if (!m_open.empty())
    {
        memcpy(m_buffer, &m_open[0], m_open.size()*sizeof(double));
    }

}

void cStrategyTemplate::onOrder(cOrderPtr p){
}

void cStrategyTemplate::onTrade(cTradePtr p){

}
void cStrategyTemplate::run(){
	int *outBegIdx;
	int *outNBElement;
	double *outReal;
	
	TA_RSI(1,100,m_buffer,2,outBegIdx,outNBElement,outReal);
	cout << "rb "<<outReal[0] << endl;

}
void cStrategyTemplate::unInit(){


}