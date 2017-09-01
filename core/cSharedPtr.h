#ifndef __CSHAREDPTR_H__
#define __CSHAREDPTR_H__

#include <stddef.h>
#include <assert.h>
#include <algorithm>
#include "cAtomicCountWin32.h"

template< class T >
class cSharedPtr
{
public:
	typedef T& reference_type;
	typedef const T& const_reference_type;
	typedef T* pointer_type;
	typedef const T* const_pointer_type;


	// construct a null shared pointer
	cSharedPtr()
	: m_pointer(0)
	, m_p_ref_count(0)
	{
	}

	// construct from actual pointer
	explicit cSharedPtr( T* p )
	{
		m_pointer = p;
		if( m_pointer )
		{
			m_p_ref_count = new cAtomicCount(1);
		}
		else
			m_p_ref_count = 0;
	}

	// copy construct, add ref to the pointer of the rhs
#if HAVE_MEMEBER_TEMPLATES
	template< class T2 >
	cSharedPtr( const cSharedPtr<T2> & other ) :
		m_pointer( other.m_pointer ),
		m_p_ref_count( other.m_p_ref_count )
#else
	cSharedPtr( const cSharedPtr& other ):
		m_pointer( other.m_pointer ),
		m_p_ref_count( other.m_p_ref_count )
#endif
	{
		AddRef();
	}

	// release ref on destruction and if ref count drops to zero
	// delete pointer and the node holding it
	~cSharedPtr()
	{
		ReleaseRef();
	}

#if HAVE_MEMBER_TEMPLATES
	template< class T2 >
	const cSharedPtr& operator = ( const cSharedPtr<T2>& other )
#else
	const cSharedPtr& operator = ( const cSharedPtr& other )
#endif
	{
		if( this == &other )
			return *this;

		// Increment the ref count of other by creating a temporary and then
		// swap this with temp. In this way, self will share other with increased ref count
		// and temporary contains what this was sharing, so when temp goes out of scope
		// the old self ref count will be decremented
		cSharedPtr( other ).Swap(*this);
		return *this;
	}

	void Bind( const cSharedPtr& other )
	{
		cSharedPtr( other ).Swap(*this);
	}

	// get ref to the actual pointer
	T& operator* ()
	{
		assert( m_pointer );
		return *m_pointer;
	}

	// get const ref to the actual pointer
	const T& operator*() const
	{
		assert( m_pointer );
		return *m_pointer;
	}

	// call through actual pointer
	inline const T* operator->() const
	{
		assert( m_pointer );
		return m_pointer;
	}

	// call through actual pointer
	inline T* operator->()
	{
		assert( m_pointer );
		return m_pointer;
	}

	// get the actual const pointer
	inline const T* Get() const
	{
		return m_pointer;
	}

	// get the actual pointer
	inline T* Get()
	{
		return m_pointer;
	}

	// get the actual pointer
	inline T*& GetRef()
	{
		return m_pointer;
	}

	// Release the pointer
	// Be careful this is a nasty function, we cannot assume the pointer we have
	// is not owned by someone else, the safest use is to call it on Unique cSharedPointers
	inline T* Release()
	{
		// if it is not a null ref pointer
		if( m_p_ref_count )
		{
			// if we own it, delete the ref count
			if( 1 == *m_p_ref_count )
			{
				delete m_p_ref_count;
			}
			else   // if we don't own it, stop sharing and return pointer
			{
				--*m_p_ref_count;
			}
		}
		T* pointer = m_pointer;
		// m_pointer = 0;
		m_p_ref_count = 0;
		return pointer;
	}

	// return refcount
	size_t RefCount() const
	{
		if( m_p_ref_count )
			return *m_p_ref_count;
		else
			return 0;
	}

	// do we have a pointer, if not we must be null shared pointer
	inline operator bool() const
	{
		return 0 != m_pointer;
	}

	// not operator should succeed if we point to a null pointer
	inline bool operator !() const
	{
		return 0 == m_pointer;
	}

	// do we have a non null pointer which has ref count of 1
	bool Unique() const
	{
		if( m_p_ref_count )
			return 1 == *m_p_ref_count;
		else
			return false;
	}

	void Swap( cSharedPtr& other )
	{
		std::swap( m_pointer, other.m_pointer );
		std::swap( m_p_ref_count, other.m_p_ref_count );
	}

	inline void AddRef()
	{
		if( m_p_ref_count && m_pointer )
			++*m_p_ref_count;
	}


protected:
	template< class T1, class T2 > friend T2 cSharedPtrCast( T1 p );

	inline void ReleaseRef()
	{
		if( m_p_ref_count == 0 )
			return;

		if( m_p_ref_count->DecAndTest() )
		{
			if( m_pointer )
				delete m_pointer;
			delete m_p_ref_count;
		}
	}

	// the pointer node we are sharing with others
	T*				m_pointer;
	cAtomicCount*	m_p_ref_count;

};

// equalty which should work if we pointing to the same pointer

template< class T1, class T2 >
inline bool operator == ( const cSharedPtr<T1>& a, const cSharedPtr<T2>& b )
{
	return a.Get() == b.Get();
}

template< class T1, class T2 >
inline bool operator != ( const cSharedPtr<T1>& a, const cSharedPtr<T2>& b )
{
	return a.Get() != b.Get();
}

template< class T1, class T2 >
inline bool operator < ( const cSharedPtr<T1>& a, const cSharedPtr<T2>& b )
{
	return a.Get() < b.Get();
}

template< class T1, class T2 >
inline T2 cSharedPtrCast( T1 p )
{
	T2 result;
	result.m_pointer = (typename T2::pointer_type)p.m_pointer;
	result.m_p_ref_count = p.m_p_ref_count;
	result.AddRef();
	return result;
}

// If there are shared copies of the argument cSharedPtr it will create new unique copy with the same value
template< class T >
inline void CopyOnWrite( cSharedPtr<T>& p )
{
	if( p.Get() && !p.Unique() )
		p = cSharedPtr<T>( new T(*p.Get() ) );
}



#endif