/***
*cVector.h - declarations/definitions for integer vector class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __CIVECTOR_H__
#define __CIVECTOR_H__

#include <stl_iostream.h>
#include <cErr.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

class cIvector
{
public:
    // Constructors
    cIvector() : size_(0), data_(NULL) {}
    cIvector( int m );
    cIvector( int value, int size );
    cIvector( const cIvector& );
    cIvector( const cIvector&, const cIvector& );

    ~cIvector();

    cIvector& operator = ( const cIvector& );
    bool operator == ( const cIvector& ) const;
    cIvector operator == ( int a ) const;

    cIvector& operator &= ( const cIvector& );        // appends a vector to this
    cIvector& operator &= ( int );                    // appends an integer to this

    bool AllEqual( const int i ) const;
    int Max() const;
    int Min() const;
    int Sum() const;
    int AbsSum() const;
    int NotEqual( const int, const int ) const; // count number of elements not equal to value val
    bool IsIncreasing() const;
    bool IsIncreasingNonStrict() const;
    void InsertWithoutResize( int n, int value, int location ); // insert element into vector without resizing: use with care
    void insert( int loc, int value );    // insert element into vector

    void drop( int index, int howmany = 1 );
    void push_back( const int value );
    int BinarySum() const;

    friend cIvector operator && ( const cIvector&, const cIvector& );
    friend ostream& operator << ( ostream& outfile, const cIvector& vec );

    int& operator[] ( int i )
    {
        yr_assert( i >= 0 && i < size_ );
        return data_[i];
    }

    int operator[] ( int i ) const
    {
        yr_assert( i >= 0 && i < size_ );
        return data_[i];
    }

    int& operator[] ( const size_t i )
    {
        yr_assert( i >= 0 && i < static_cast<size_t>( size_ ) ); 
        return data_[i];
    }

    int operator[] ( const size_t i ) const
    {
        yr_assert( i >= 0 && i < static_cast<size_t>( size_ ) );
        return data_[i];
    }

    void initialize( int val = 0 );
    void resize( int newsize_ = 0 );
    void resize( int newsize_, int value );
    int interval( int t, int adj = 0, int last = 0 ) const;
    int location( int t, int last = 0 );
    cIvector& sortunique();
    int getSize() const { return size_; }
    operator int*() { return data_; }
    operator const int*() const { return data_; }

    static void Swap( cIvector& a, cIvector& b )
    {
        int* temp1 = a.data_;
        a.data_ = b.data_;
        b.data_ = temp1;

        int temp2 = a.size_;
        a.size_ = b.size_;
        b.size_ = temp2;
    }



private:
    void _allocate();
    int* data_;
    int size_;
};

inline int sum( const cIvector& i ) { return i.Sum(); }

cIvector merge( const cIvector& a1, const cIvector& a2 );
int FindIndex( const cIvector& a, const int& b );
cIvector index_sort( const cIvector& ra );
cIvector stable_index_sort( const cIvector& sVec );
cIvector subvector( const cIvector& a, int from, int to );

extern const cIvector theIvector;


#endif
