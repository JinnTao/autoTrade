#ifndef __CARRAY_CPP__
#define __CARRAY_CPP__

#include <string.h>
#include <cArray.h>
#include <cIvector.h>

#ifdef ARRAY_DEBUG
#include <stdio.h>

template< class T >
long cArray< T >::_create_array = 0;

template< class T >
long cArray< T >::_delete_array = 0;
#endif

// Default Constructor
template< class T >
cArray< T >::cArray() : data_()
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

// Constructor with size
template< class T >
cArray< T >::cArray( int size ) : data_( ( size_t )size )
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

// Constructor with size
template< class T >
cArray< T >::cArray( size_t size ) : data_( size )
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

// Constructor with size and initialization of values
// memberwise copy
template< class T >
cArray< T >::cArray( const T& val, int size ) : data_( ( size_t )size, val )
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

template< class T >
cArray< T >::cArray( const T& val, size_t size ) : data_( size, val )
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

// copy constructor
// bulk copy
template< class T >
cArray< T >::cArray( const cArray< T >& array ) : data_( array.data_ )
{
#ifdef ARRAY_DEBUG
    _create_array++;
#endif
}

// Destructor
template< class T >
cArray< T >::~cArray()
{
#ifdef ARRAY_DEBUG
    _delete_array++;
    if( _delete_array >= _create_array )
        printf( "arrays: %d created, %d deleted. \n", _create_array, _delete_array );
#endif
}

// Methods for class cArray
//
// resize cArray< T >
template< class T >
cArray< T >& cArray< T >::resize( int size )
{
    if( size < 0 )
    {
        yr_error( "cArray< T >::resize( %d, value )", size );
    }
    if( static_cast< int >( data_.size() ) < size )
    {
        data_.insert( data_.end(), size - data_.size(), T() );
    }
    else if( size < static_cast< int >( data_.size() ) )
    {
        /*
            Ensure that the memory is deallocated and released if the size is reduced.
            STL vector functions erase/resize does not release memory when size decreases,
            so only way to ensure reallocation is to swap
        */
        std::vector< T > temp( size );
        std::copy( data_.begin(), data_.begin() + size, temp.begin() );
        data_.swap( temp );
    }
    return *this;
}

template< class T >
cArray< T >& cArray< T >::resize( int size, const T& value )
{
    // note: this method does not preserve the values, it emulates prev, non stl based array
    if( size < 0 )
    {
        yr_error( "cArray< T >::resize( %d, value ).", size );
    }
    else if( data_.size() == ( size_t )size )
    {
        initialize( value );
    }
    else
    {
        std::vector< T > new_data( ( size_t )size, value );
        data_.swap( new_data );
    }
    return *this;
}

// Set all elements to 'value'
template< class T >
void cArray< T >::initialize( const T& value )
{
    const size_t sz = data_.size();
    for( size_t i = 0; i < sz; ++i )
    {
        data_[i] = value;
    }
}

template< class T >
void cArray< T >::insert( int index, const T& item )
{
    // note: code below is not strictly correct. if given index is > size(0, it will expand
    // data size by one and ignore the value, we keep it for backwards compatibility ( for
    // the time being )
    int size = ( int )data_.size();

    // resize array
    resize( size + 1 );

    // move existing elements up one
    for( int i = size - 1; i >= index; i-- )
        data_[i + 1]  = data_[i];

    data_[index] = item;
}

template< class T >
cArray< T > insert( const int index, const T& item, const cArray< T >& array )
{
    cArray< T > newarray = array;
    newarray.insert( index, item );
    return newarray;
}

template< class T >
void cArray< T >::drop( int index, int numitems )
{
    if( numitems == 0 )
        return;
    int size = ( int )data_.size() - numitems;
    for( int i = index; i >= 0 && i < size; i++ )
        data_[i] = data_[i + numitems];

    resize( size >= 0 ? size : 0 );
}

template< class T >
void cArray< T >::erase( iterator it_begin, iterator it_end )
{
    // note cArray iterator is not vector::iterator to fit in with existing code.
    // cArray iterators are pointers to elements, empty arrays have being() and end() equal zero
    if( it_begin && size() )
    {
        data_.erase( data_.begin() + ( it_begin - &data_[0] ), data_.begin() + ( it_end - &data_[0] ) );
    }
}

template< class T >
cArray< T >& cArray< T >::operator &= ( const T& item )
{
    data_.push_back( item );
    return *this;
}

template< class T >
cArray< T >& cArray< T >::operator &= ( const cArray< T >& array )
{
    data_.insert( data_.end(), array.begin(), array.end() );
    return *this;
}

// memory allocation check
// function that checks if pointer is null
template< class T >
void cArray< T >::testP() const
{
}

template< class T >
void cArray< T >::Swap( cArray< T >& a, cArray< T >& b )
{
    a.data_.swap( b.data_ );
}

template< class T >
void cArray< T >::swapElements( const int i, const int j )
{
    yr_assert( i >= 0 && i < static_cast< int >( data_.size() ) );
    yr_assert( j >= 0 && j < static_cast< int >( data_.size() ) );

    const T tmp( data_[i] );
    data_[i] = data_[j];
    data_[j] = tmp;
}

template< class T >
class cArrayIndexSortHelper
{
public:
    cArrayIndexSortHelper( const cArray< T >& sVec ) : _vec( sVec )
    {
    }
    int operator() ( int a, int b ) const
    {
        return( _vec[a] < _vec[b] );
    }
private:
    const cArray< T >& _vec;
};

template< class T >
void InsertOrdered( cArray< T >& sVec, const T& d )
{
    if( sVec.getSize() == 0 || d > sVec[ sVec.getSize() - 1 ] )
        sVec.push_back( d );
    else if( d < sVec[ 0 ] )
        sVec.insert( 0, d );
    else
    {
        int p = sVec.interval( d, 1 );
        if( sVec[p] < d )
            sVec.insert( p + 1, d );
    }
}

// Find the interval in which 'val' lies, i.e. return i such that
// array[i] <= val < array[i+1], with the following caveats
// if val < array[first[], it returns first
// if val >= array[size-1], it returns adj ? size-1 : size-2
template< class T >
int cArray< T >::interval( const T& val, const bool adj ) const
{
    const size_t n = data_.size();
    yr_assert( n > 0 );

    // check the ends first
    if( val >= data_[n-1] )
        return adj ? ( int )n-1 : (int)n-2;
    if( val < data_[0] )
        return 0;

    int il = 0;
    int ih = ( int )n - 1;

    while( ih > il + 1 )
    {
        int midpoint = ( il +ih ) / 2;
        if( val < data_[midpoint] )
            ih = midpoint;
        else
            il = midpoint;
    }
    return il;
}

template< class T >
cIvector index_sort_array( const cArray< T >& sVec )
{
    int n = sVec.getSize();
    cIvector index( n );
    for( int i = 0; i < n; ++i )
        index[i] = i;

    cArrayIndexSortHelper< T > indexSortHelper( sVec );
    sort( (int*)index, (int*)index + n, indexSortHelper );
    return index;
}


template< class T >
cArray< T > subvector( const cArray< T >& a, int from, int to )
{
    int new_size = max( to - from, 0 ) + 1;
    cArray< T > temp( new_size );

    for( int i = 0; i < new_size; ++i )
    {
        temp[i] = a[from+i];
    }
    return temp;
}

#endif
