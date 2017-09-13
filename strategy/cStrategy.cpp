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
	while(this->m_status){
		
		this->run();
		Sleep(this->m_timeSpan);

	}
	return 0;

}