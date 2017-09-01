#ifndef __YR_TICKTIME_TEST__
#define __YR_TICKTIME_TEST__

void RunTickTimeTest();

//different ticktime tests

// test the function cTickTime::DoubleToTickTime without millisec
void TickTimeTestDoubleToTickTime();

// test the function cTickTime::DoubleToTickTime with millisec
void TickTimeTestDoubleToTickTimeWithMilliSecond();

// test the cVector::location or cVector::interval 
void TickTimeTestVectorLocation();


#endif