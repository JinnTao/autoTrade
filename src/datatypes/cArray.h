#ifndef __CARRAY_H__
#define __CARRAY_H__

#include <vector>
#include <algorithm>
#include <math_defs.h>
#include <cErr.h>

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

// Uncomment the next line if you want to know
// how many strings are created and destroyed
// when you run your program
// #define ARRAY_DEBUG

class cIvector;

template < class T >
class cArray
{
public:
    // note: we cannot use vector iterators as they are not always implemented as T*
    // some users rely on this
    typedef typename std::vector< T >::const_reference const_reference;
    typedef typename std::vector< T >::reference reference;
    typedef typename std::vector< T >::value_type value_type;

    cArray();
    cArray( int );
    cArray( size_t );
    cArray( const cArray< T >& );
    cArray( const T&, int );
    cArray( const T&, size_t );

    ~cArray();

    // note: we cannot return T& and const T& because
    // std::vector<bool> is specialized and does not return T& and const T&
    reference operator[] ( int i )
    {
        yr_assert( i >= 0 && i < (int)data_.size() );
        return data_[(size_t)i];
    }

    const_reference operator[] ( int i ) const
    {
        yr_assert( i < (int)data_.size() );
        return data_[i];
    }

    reference getLast()
    {
        yr_assert( data_.size() > 0 );
        return data_[(size_t)(data_.size()-1)];
    }

    const_reference getLast() const
    {
        yr_assert( data_.size() > 0 );
        return data_[data_.size()-1];
    }

    cArray< T >& operator = ( const cArray< T >& );

    T* getArray()
    {
        return data_.empty() ? 0 : &data_[0];
    }

    const T* getArray() const
    {
        return data_.empty() ? 0 : &data_[0];
    }

    int getSize() const
    {
        return (int)data_.size();
    }

    cArray< T >& resize( int size );
    cArray< T >& resize( size_t size ) { return resize( (int)size ); }
    // warning, this is not like stl resize, it really is resize_and_fill_all
    cArray< T >& resize( int size, const T& value );
    // resize array to newsize keep old values and fill new ones with value's, just like stl
    void resize_keep( int newsize, const T& value = T() )
    {
        data_.resize( (size_t)newsize, value );
    }

    void initialize( const T& value );    // Set all elements to 'value'

    void reserve( size_t size ) { data_.reserve( size ); }

    cArray< T >& operator &= ( const T& item );
    cArray< T >& operator &= ( const cArray< T >& item );

    void operator ++ ()    { this->operator &= ( T() ); }
    void insert( int index, const T& item );
    cArray< T > insert( const int index, const T& item, const cArray< T >& array );
    void drop( int index, int numitems = 1 );

    static void Swap( cArray< T >& a, cArray< T >& b );

    typedef T* iterator;
    typedef const T* const_iterator;

    iterator begin() { return data_.empty() ? 0 : &data_[0]; }
    const_iterator begin() const { return data_.empty() ? 0 : &data_[0]; }

    iterator end() { return begin() + size(); }
    const_iterator end() const { return begin() + size(); }

    iterator last() { return data_.empty() ? 0 : begin() + size() - 1; }
    const_iterator last() const { return data_.empty() ? 0 : begin() + size() - 1; }

    iterator find( const T& item ) { return std::find( begin(), end(), item ); }
    const_iterator find( const T& item ) const { return std::find( begin(), end(), item ); }

    int size() const { return ( int )data_.size(); }
    size_t capacity() { return data_.capacity(); }
    void swap( cArray< T >& other ) { data_.swap( other.data_ ); }
    void swapElements( const int i, const int j );
    void clear() { data_.clear(); }
    void push_back( const T& value ) { data_.push_back( value ); }
    void erase( iterator it_begin, iterator it_end );
    T* data() { return data_.empty() ? 0 : &data_[0]; }
    const T* data() const { return data_.empty() ? 0 : &data_[0]; }

    // Find the interval in which 'val' lies, i.e. return i such that
    // array[i] <= val < array[i+1], with the following caveats:
    // if val < array[0], it returns 0
    // if val >= array[size-1], it returns adj ? size - 1:size-2
    int interval( const T& val, const bool adj ) const;

private:

    void testP() const;

    std::vector< T > data_;

#ifdef ARRAY_DEBUG
    static long _create_array;
    static long _delete_array;

#endif

};

template< class T >
cIvector index_sort_array( const cArray< T >& ra );

template< class T >
cArray< T > subvector( const cArray< T >&a, int from, int to );

template< class T >
void InsertOrdered( const cArray< T >& sVec, const T& d );

#include "cArray.cpp"


#endif