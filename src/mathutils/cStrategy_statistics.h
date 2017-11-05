#ifndef __CSTRATEGY_STATISTICS_H__
#define __CSTRATEGY_STATISTICS_H__

class cVector;
class cTickTime;

void datawindow( const cVector&, const cVector&, const cTickTime&, const cTickTime&, cVector& );


double sharpratio( const cVector&, double scale = 1.0 );
double drawdown( const cVector&, double& peak );

// Chart Technical Indicators
void movavg( const cVector&, cVector&, int lag, double alpha = 0.0, bool isexp = false );





#endif
