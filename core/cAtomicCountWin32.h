#ifndef __CATOMICCOUNTWIN32_H__
#define __CATOMICCOUNTWIN32_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#ifdef small
#undef small
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "cNonCopyable.h"

class cAtomicCount : cNonCopyable
{
public:
	explicit cAtomicCount( long v ) : m_count(v)
	{
	}

	long operator ++ ()
	{
		return InterlockedIncrement( const_cast<long*>( &m_count ) );
	}

	long operator -- ()
	{
		return InterlockedDecrement( const_cast<long*>( &m_count ) );
	}

	int DecAndTest()
	{
		return InterlockedDecrement( const_cast<long*>( &m_count ) ) == 0;
	}

	operator long() const
	{
		return m_count;
	}


private:
	volatile long m_count;
};

#endif