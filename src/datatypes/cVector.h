/***
*cVector.h - declarations/definitions for vector class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <string.h>
#include <math_defs.h>
#include <stl.h>


#ifdef WIN32
// Force the following functions to be generated inline...
#pragma intrinsic( abs, fabs, labs, memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen )
#endif


#include <cIvector.h>


inline double VSum( int size, const double* data )
{
    double sum = 0.0;
    for( const double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
        sum += *ptr;

    return sum;
}


class cVector
{
public:
    // Constructors
    cVector();
    explicit cVector( int );
    cVector( double, int );
    cVector( const double*, int );
    cVector( double*, int, bool );
    cVector( const cVector& );
    cVector( const cIvector& );
    // creates a vector that concatenates data of 2 other vectors
    cVector( const cVector&, const cVector& );
    void initialize( double );

    // Destrcutor
    ~cVector();

    // operators
    cVector& operator = ( const cVector& );
    bool operator == ( const cVector& ) const;
    bool operator != ( const cVector& ) const;
    cVector& operator += ( const cVector& );
    cVector& operator += ( double );
    cVector& operator += ( int );
    //
    cVector& operator -= ( const cVector& );
    cVector& operator -= ( double );
    cVector& operator -= ( int );
    //
    cVector& operator *= ( const cVector& );
    cVector& operator *= ( double );
    cVector& operator *= ( int );
    //
    cVector& operator /= ( const cVector& );
    cVector& operator /= ( double );
    cVector& operator /= ( int );
    //
    // appends a vector to this
    cVector& operator &= ( const cVector& );
    cVector& operator &= ( double );
    cVector operator - () const { return ( -1.0 ) * ( *this ); }

    cVector& Min( double );
    cVector& Max( double );
    cVector& Min( const cVector& );
    cVector& Max( const cVector& );
    cVector& Fabs();
    cVector& Exp();
    cVector& Log( bool check_for_fuzz = true );
    cVector& Sin();
    cVector& ASin();
    cVector& Cos();
    cVector& Normal();
    cVector& NormalInv();
    cVector& Sqrt();
    cVector& Sqr();

    cVector& Pow( const cVector& );
    cVector& Pow( double );

    cVector& Inverse( double );
    cVector& AddProduct( const double, const cVector& );
    cVector& Divide( const cVector& );
    cVector& DivideThisBy( const cVector&, double );
    cVector& DivideThisBySmooth( const cVector&, double, double );
    cVector& DivideThisBy( double, double );

    cVector Fabs( const cVector& );
    cVector Max( const cVector&, const cVector& );
    cVector Min( const cVector&, const cVector& );
    cVector Max( const cVector&, double );
    cVector Max( double, const cVector& );
    cVector Min( const cVector&, double );
    cVector Min( double, const cVector& );

    double Max() const;
    double Min() const;
    double Sum() const
    {
        return VSum( size_, data_ );
    }
    double Avg() const
    {
        return size_? Sum() / size_ : 0.0;
    }

    double Norm() const;    // sqrt of the sum of the squares of the elements
    int interval( double t, int adj, int first = 0 ) const;    // returns leftmost peg
    int location( double t, int first = 0 ) const;    // return closet peg
    int upper_bound( double t ) const;    // returns highest position where t can be inserted (using insert() ) such that vector remains sorted
    int lower_bound( double t ) const;    // returns closest position where t can be inserted (using insert() ) such that vector remains sorted
    void Bounds( const double x, int& lower, int& upper ) const;    // finds bounds on the position of x within vector
    void InsertWithoutResize( int, double, int );    // insert element into vector without resize: use with care
    double Interp( int&, int, const cVector& xv, double x, int logInterp, double& delta ) const;    // liner log interp from zcurve
    double Interp( int, const cVector& xv, double x, int logInterp = 0 ) const;
    void insert( int, double );    // insert element into vector
    void insert( int, int, double );    // insert elements into vector
    void push_back( double );    // add as new last element
    void drop( int );            // drops an element from a vector
    void drop( int, int );        // drops a range of elements from a vector


    // Element operator
    double& operator[] ( int i )
    {
        yr_assert( i >=0 && i < size_ );
        return data_[i];
    }
    // Element operator - const version
    const double& operator[] ( int i ) const
    {
        yr_assert( i >=0 && i < size_ );
        return data_[i];
    }
    // Element operator - using size_t
    double& operator[] ( const size_t i )
    {
        yr_assert( i < static_cast<size_t>( size_ ) );
        return data_[i];
    }
    // Element operator - const version using size_t
    const double& operator[] ( const size_t i ) const
    {
        yr_assert( i < static_cast<size_t>( size_ ) );
        return data_[ i ];
    }
    
    void clear()
    {
        resize( (int)0 );
    }

    void resize( size_t newsize_ )
    {
        resize( (int)newsize_ );
    }

    void resize( int newsize_ );
    void resize( size_t newsize, double value, size_t exactSize = 0 )
    {
        resize( ( int )newsize, value, ( int )exactSize );
    }
    void resize_keep( int newSize, double value, int exactSize = 0 );
    void resize( int, double, int exactSize = 0 );
    void dirty_resize( int );
    void exact_resize( int newsize_ = -1 );    // the default argument of -1 makes it resize the vector to its current apparent size
    friend ostream& operator << ( ostream& outfile, const cVector& vec );
    void reserve( int );

    friend cVector operator + ( const cVector&, double );            // addition by scalar
    friend cVector operator + ( double, const cVector& );            // addition by scalar
    friend cVector operator + ( const cVector&, int );                // addition by integer
    friend cVector operator + ( int, const cVector& );                // addition by integer

    friend cVector operator && ( const cVector&, const cVector& );    // concatenation
    friend cVector operator && ( double, const cVector& );            // concatenate vect to a double
    friend cVector operator && ( const cVector&, double );            // concatenate double to vect

    friend cVector operator - ( const cVector&, double );            // subtraction by scalar
    friend cVector operator - ( double, const cVector& );            // subtraction by scalar
    friend cVector operator - ( const cVector&, int );                // subtraction by integer
    friend cVector operator - ( int, const cVector& );                // subtraction by integer

    friend cVector operator * ( const cVector&, double );            // multiplication by scalar
    friend cVector operator * ( double, const cVector& );            // multiplication by scalar
    friend cVector operator * ( const cVector&, int );                // multiplication by integer
    friend cVector operator * ( int, const cVector& );                // multiplication by integer

    friend cVector operator / ( const cVector&, double );            // division by scalar
    friend cVector operator / ( double, const cVector& );            // division by scalar
    friend cVector operator / ( const cVector&, int );                // division by integer
    friend cVector operator / ( int, const cVector& );                // division by integer

    friend cVector operator > ( const cVector&, double );
    friend cVector operator < ( const cVector&, double );
    friend cVector operator >= ( const cVector&, double );
    friend cVector operator <= ( const cVector&, double );

    friend cVector operator > ( double, const cVector& );
    friend cVector operator < ( double, const cVector& );
    friend cVector operator >= ( double, const cVector& );
    friend cVector operator <= ( double, const cVector& );

    double getLast() const
    {
        yr_assert( size_ > 0 );
        return data_[ size_-1 ];
    }

    int getSize() const
    {
        return size_;
    }
    size_t size() const
    {
        return (size_t)size_;
    }
    operator double*()
    {
        return data_;
    }
    operator const double*() const
    {
        return data_;
    }
    double* data()
    {
        return data_;
    }
    const double* data() const
    {
        return data_;
    }

    static int _real_size( int );
    bool owns_data() const
    {
        return owns_data_;
    }

    // STL-like functions

    typedef double* iterator;
    typedef const double* const_iterator;

    iterator begin()
    {
        return data_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }

//#if defined(GNU) || defined(WIN32)
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
//#endif

    reverse_iterator rbegin()
    {
        return ( reverse_iterator(end()) );
    }

    const_reverse_iterator rbegin() const
    {
        return ( const_reverse_iterator(end()) );
    }

    reverse_iterator rend()
    {
        return ( reverse_iterator(begin()) );
    }

    const_reverse_iterator rend() const
    {
        return ( const_reverse_iterator(begin()) );
    }

    void erase( iterator it_begin, iterator it_end );
    void sort();    // sort the vector
    cVector& sortunique();    // sort the vector and remove duplicates
    cVector& remove_duplicates( double fuzz = FUZZ );
    void Copy( const cVector& in, int size )
    {
        yr_assert( size >= 0 && size <= size_ && size <= in.size_ );
        memcpy( data_, in.data(), size * sizeof( double ) );
    }

    static void Swap( cVector& a, cVector& b )
    {
        double* temp1 = a.data_;
        a.data_ = b.data_;
        b.data_ = temp1;

        int temp2 = a.size_;
        a.size_ = b.size_;
        b.size_ = temp2;

        temp2 = a.real_size;
        a.real_size = b.real_size;
        b.real_size = temp2;

        bool b_temp = a.owns_data_;
        a.owns_data_ = b.owns_data_;
        b.owns_data_ = b_temp;
    }

    void swapElements( int i, int j )
    {
        yr_assert( i >= 0 && i < size_ );
        yr_assert( j >= 0 && j < size_ );

        const double tmp = data_[i];
        data_[i] = data_[j];
        data_[j] = tmp;
    }

    cVector& merge( double, double tolerance = 0.0, bool replace = true );
    cVector& merge( const cVector&, double tolerance = 0.0 );

    void reverse()
    {
        if( getSize() <= 0 )
            return;
        std::reverse( data_, data_ + size_ );
    }

    int contains( double val, double tol = 0.0 ) const;
    int find( const double val, const double tol = 0.0, const int start = 0 ) const;

    int NumSmaller( int n, double x ) const; // counts number of elements less than x.
    int FirstGreater( int n, double val, int first = 0 ) const; // finds first element greater than val
    bool AllEqual( int n = 0, double fuzz = FUZZ ) const; // check if all elements are equals
    bool IsIncreasing( int n ) const; // is the vector strictly increasing
    bool IsIncreasingNonStrict( int n ) const;    // is the vector increasing
    bool IsZeroVector() const;    // true if the vector is empty or contains all zeros (within FUZZ)
    bool IsEqual( double x ) const;    // true if the vector is empty or contant equal to x.
    bool IsSameSign() const;    // true if all elements are positive or all elements are negative



public:
    static double* malloc_doubles( size_t s )
    {
        return new double[ s ];
    }

    static void free_doubles( double* p )
    {
        delete [] p;
    }

protected:
    void _allocate( int s );
    void _reallocate( int s, int exactSize = 0 );
    void _reallocate_and_preserve( int s, double value, int exactSize = 0 );

    double *data_;
    bool    owns_data_;
    int        size_;
    int        real_size;
};

cVector operator + ( const cVector& lVec, const cVector& rVec );
cVector operator - ( const cVector& lVec, const cVector& rVec );
cVector operator * ( const cVector& lVec, const cVector& rVec );
cVector operator / ( const cVector& lVec, const cVector& rVec );
cVector operator > ( const cVector& lVec, const cVector& rVec );
cVector operator < ( const cVector& lVec, const cVector& rVec );
cVector operator >= ( const cVector& lVec, const cVector& rVec );
cVector operator <= ( const cVector& lVec, const cVector& rVec );


inline cVector merge( const cVector& a, double x, double tolerance = 0.0 )
{
    return cVector( a ).merge( x, tolerance );
}

inline cVector merge( const cVector& a1, const cVector& a2, double tolerance = 0.0 )
{
    return cVector( a1 ).merge( a1, tolerance );
}

cVector power( const cVector& x, const double& exponent );
cVector pow( const cVector&, double );
cVector exp( const cVector& );
cVector log( const cVector& );
cVector sin( const cVector& );
cVector cos( const cVector& );
cVector asin( const cVector& );
cVector normal( const cVector& );
cVector normal_inv( const cVector& );
double standard_dev( const cVector&, double& );
double variance( const cVector& );
double mean( const cVector& );

cVector sequence( int, int, int );
cIvector index_sort( const cVector& sVec );
cIvector stable_index_sort( const cVector& sVec );
cIvector index_sort( int size, const double* data );
void dirty_index_sort( const cVector& sVec, cIvector& index, int n = -1 );
inline double sum( const cVector& c ) { return c.Sum(); }

cVector yr_divide( const cVector& numerator, const cVector& denominator, const double& backup );
cVector yr_divide( const cVector& numerator, const double& denominator, const double& backup );
cVector yr_divide( const double& numerator, const cVector& denominator, const double& backup );

cVector sqrt( const cVector& vec );
int minimize_sort_index( const cVector& vec, const cIvector& sort_index, int start_index );
cVector subvector( const cVector&, int from, int to );


// double* utilities which can be used buffer of doubles
void VAddInPlace( int size, double* data, double* other );
void VAddInPlace( int size, double* data, double v );
void VSubInPlace( int size, double* data, double* other );
void VSubInPlace( int size, double* data, double v );
void VMultInPlace( int size, double* data, double* other );
void VMultInPlace( int size, double* data, double v );
void VDivInPlace( int size, double* data, double* other );
void VDivInPlace( int size, double* data, double v );
void VPowInPlace( int size, double* data, double* other );
void VPowInPlace( int size, double* data, double v );
void VFabsInPlace( int size, double* data );
void VExpInPlace( int size, double* data );
void VLogInPlace( int size, double* data, bool check_for_fuzz = true );
void VAsinInPlace( int size, double* data );
void VSinInPlace( int size, double* data );
void VCosInPlace( int size, double* data );
void VNormalInPlace( int size, double* data );
void VNormalInvInPlace( int size, double* data );
void VSqrtInPlace( int size, double* data );
void VSqrInPlace( int size, double* data );
//double VSumProdDefault( size_t size, const double* lv, const double* rv );
void VInvInPlace( int size, double* data );
double VMax( int size, const double* data );
double VMin( int size, const double* data );
double VMean( int size, const double* data );
double VVariance( int size, const double* data );
double VSumProd( size_t size, const double* lv, const double* rv );
void VScaledAdd( size_t size, double a, const double* x, double* y );

extern const cVector theVector;


#endif

