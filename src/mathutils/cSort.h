#ifndef __CSORT_H__
#define __CSORT_H__

#ifdef WIN32
#pragma warning( disable: 4786 )
#endif

template< class T >
void QSort( T* array, int size, int direction );

#include <cSort.cpp>

#endif
