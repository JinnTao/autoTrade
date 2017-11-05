#ifndef __CSORT_CPP__
#define __CSORT_CPP__

#include "cErr.h"
#include "stl.h"
//#include "yr_assert.h"

template< class T >
class cSortCompareUp
{
public:
	int operator()( const T& i, const T& j )
	{
		return ( i < j );
	}
};

template< class T >
class cSortCompareDown
{
public:
	int operator()( const T& i, const T& j )
	{
		return ( i < j );
	}
};

// Templated function QSort which will call down to
// operating system qsort if size > threshlod, otherwise
// do a bubble sort. This is actually quicker for small arrays
// as long as the elements are in some kind of order already
// if not, just set the threshold to 0
// The direction is > 0 for ascending, < 0 for descending

template< class T >
void QSort( T* array, int size, int direction )
{
	int threshold = 1000;

	// For array more than threshold elements long, use qsort
	if( size >= threshold )
	{
		if( direction > 0 )
		{
			cSortCompareUp< T > sortCompareUp;
			sort( array, array + size, sortCompareUp );
		}
		else if( direction < 0 )
		{
			cSortCompareDown< T > sortCompareDown;
			sort( array, array + size, sortCompareDown );
		}
		else
			yr_error( "QSort direction must be non-zero" );

		return;
	}

	// This MUCH faster than the operating system "qsort"
	// but only for small-ish arrays which are already almost sorted
	int i = 0;
	int j;
	int end = size - 1;
	T temp;

	// This is a one-pass sort
	// When we reach the end, we've finished

	//
	// Code is duplicated for speed
	//
	if( direction > 0 )
	{
		while( i < end )
		{
			j  = i;
			if( array[i+1] < array[i] )
			{
				temp = array[i+1];
				while( i >= 0 && temp < array[i] )
				{
					array[i+1] = array[i];
					i--;
				}
				array[i+1] = temp;
			}
			i = j + 1;
		}
	}
	else if( direction < 0 )
	{
		while( i < end )
		{
			j = i;
			if( array[i+1] > array[i] )
			{
				temp = array[i+1];
				while( i >=0 && temp > array[i] )
				{
					array[i+1] = array[i];
					i--;
				}
				array[i+1] = temp;
			}
			i = j + 1;
		}
	}
	else
		yr_error( "QSort direction must be non-zero!" );
}

#endif
