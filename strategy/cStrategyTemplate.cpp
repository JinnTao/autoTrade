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
}

void cStrategyTemplate::onOrder(cOrderPtr p){
}

void cStrategyTemplate::onTrade(cTradePtr p){

}
void cStrategyTemplate::run(){



	cStrategy::run();
}
