/***
*cVector.cpp
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#include <cVector.h>

#include <stl_iostream.h>
#include <math_defs.h>
#include <cSort.h>
//#include "normal.h"
//#include "math_utils.h"


using namespace std;

#pragma warning( disable:4996 )


// Quantize the real size_ of the array to the next power of 2
int cVector::_real_size( int x )
{
	if( x == 0 ) return 0;
	if( x == 1 ) return 1;
	if( x < 128 ) return 128;
	if( x > 4096 ) return ( ( x | 255 ) + 1 );
	x >>= 6;

	int i = 1;
	while( x > 1 )
	{
		x >>= 1;
		i++;
	}
	return( 64 << i );
}

void cVector::_allocate( int s )
{
	if( !owns_data() )
		yr_error( "_allocate not allowed on a vector with wraps data" );

	size_ = s;
	if( s > 0 )
	{
		real_size = _real_size( s );
		data_ = malloc_doubles( real_size );
		if( data_ == NULL )
			yr_error( "cVector: No more memory for vector objects!" );
	}
	else if( s == 0 )
	{
		data_ = NULL;
		real_size = 0;
	}
	else if( s < 0 )
	{
		yr_error( "_allocate: negative size " );
	}
}

void cVector::_reallocate( int s, int exactSize )
{
	if( size_ == s && !exactSize )
		return;

	if( !owns_data() )
		yr_error( "_allocate not allowed on a vector with wraps data" );

	if( s > real_size || ( exactSize && s > 0 ) )
	{
		if( data_ && exactSize != 2 )
			free_doubles( data_ );

		real_size = exactSize ? s : _real_size( s );
		data_ = malloc_doubles( real_size );
		if( data_ == NULL )
			yr_error( "cVector: No more memory for vector objects!" );
	}
	else if( s == 0 )
	{
		if( data_ )
			free_doubles( data_ );
		real_size = 0;
		data_ = NULL;
	}
	else if( s < 0 )
	{
		yr_error( "_reallocate: negative size " );
	}

	size_ = s;
}

namespace
{
	struct vector_buffer_guard
	{
		vector_buffer_guard( double* buf ) : m_buf( buf ){}
		~vector_buffer_guard()
		{
			if( m_buf )
				cVector::free_doubles( m_buf );
		}

		void release() { m_buf = 0; }
		double* m_buf;
	};
}

void cVector::_reallocate_and_preserve( int s, double value, int exactSize )
{
	
	if( size_ == s && !exactSize )
		return;

	if( !owns_data() )
		yr_error( "cVector::_reallocate_and_preserve not allowed on a vector which wraps data" );

	vector_buffer_guard guard( data_ );
	double* curr_data = data_;
	int curr_size = size_;

	if( s > real_size || ( exactSize && s > 0 ) )
	{
		real_size = exactSize ? s : _real_size( s );
		data_ = malloc_doubles( real_size );
		if( data_ == NULL )
			yr_error( "cVector: No more memory for vector objects!" );
	}
	else if( s == 0 )
	{
		real_size = 0;
		data_ = NULL;
	}
	else if( s < 0 )
	{
		yr_error( "_reallocate_and_preserve: negative size " );
	}
	if( curr_data && curr_data != data_ && s > curr_size )
	{
		std::copy( curr_data, curr_data + curr_size, data_ );
	}

	if( s > curr_size )
		fill( data_ + curr_size, data_ + s, value );

	if( data_ == curr_data )
		guard.release();

	size_ = s;
	
}


cVector::cVector()
: size_( 0 )
, real_size( 0 )
, data_( NULL )
, owns_data_( true )
{
}

cVector::cVector( int m )
: owns_data_( true )
{
	_allocate( m );
	memset( data_, 0, size_ * sizeof( double ) );
}

cVector::cVector( double v, int m )
: owns_data_( true )
{
	_allocate( m );
	for( double* ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ptr++ )
		*ptr = v;
}

cVector::cVector( const double* v, int m ) 
: owns_data_( true )
{
	_allocate( m );
	memcpy( data_, v, size_ * sizeof( double ) );
}

cVector::cVector( double* t, int m, bool wrap_data )
{
	if( wrap_data )
	{
		data_ = t;
		size_ = real_size = m;
		owns_data_ = false;
	}
	else
	{
		_allocate( m );
		memcpy( data_, t, size_ * sizeof( double ) );
		owns_data_ = true;
	}
}

cVector::cVector( const cVector& vec )
: owns_data_( true )
{
	yr_assert( this != &vec );
	_allocate( vec.getSize() );
	memcpy( data_, vec.data_, size_ * sizeof( double ) );

}

cVector::cVector( const cIvector& vec )
: owns_data_( true )
{
	_allocate( vec.getSize() );
	for( int i = 0; i < size_; ++i )
		data_[i] = vec[i];
}

// concatenate constructor
cVector::cVector( const cVector& lVec, const cVector& rVec ) 
: owns_data_( true )
{
	yr_assert( this != &lVec && this != &rVec );
	_allocate( lVec.size_ + rVec.size_ );
	memcpy( data_, lVec.data_, lVec.size_ * sizeof( double ) );
	memcpy( data_ + lVec.size_, rVec.data_, rVec.size_ * sizeof( double ) );
}

void cVector::initialize( double val )
{
	if( 0.0 == val )
	{
		memset( data_, 0, size_ * sizeof( double ) );
	}
	else
	{
		for( double *ptr = data_, *ptr_end = data_ + size_; ptr != ptr_end; ++ptr )
			*ptr = val;
	}
}

bool cVector::operator == ( const cVector& a ) const
{
	if( this == &a )
		return true;

	if( a.size_ != size_ )
		return false;

	for( int i = 0; i < a.getSize(); ++i )
	{
		if( fabs( a.data_[i] - data_[i] ) > MFUZZ )
			return false;
	}

	return true;
}

bool cVector::operator != ( const cVector& a ) const
{
	return !( *this == a );
}

// assignment operator
cVector& cVector::operator = ( const cVector& vec )
{
	if( &vec != this )
	{
		_reallocate( vec.size_ );
		if( size_ )
			memcpy( data_, vec.data_, size_ * sizeof( double ) );
	}
	return *this;
}

// destructor
cVector::~cVector()
{
	if( !owns_data() )
		return;
#ifdef _DEBUG
	if( data_ == (double*) - 1 )
		yr_error( "object already deleted" );
#endif

	if( data_ )
		free_doubles( data_ );

#ifdef _DEBUG
	data_ = ( double* ) - 1;
	size_ = -1;
	real_size = -1;
#endif
}


// resize vector to newsize_
void cVector::resize( int newsize_ )
{
	if( newsize_ == size_ )
		return;
	
	if( !owns_data() )
		yr_error( "resize not allowed on a vector which wraps data" );

	if( newsize_ > 0 )
	{
		int old_size = size_;
		if( newsize_ > real_size )
		{
			double* old_p = data_;
			_allocate( newsize_ );
			if( old_size > 0 )
				memcpy( data_, old_p, old_size * sizeof( double ) );
			if( old_p )
				free_doubles( old_p );
		}
		else
		{
			size_ = newsize_;
		}

		if( newsize_ > old_size )
			memset( data_ + old_size, 0, ( newsize_ - old_size ) * sizeof( double ) );
	}
	else if( newsize_ == 0 )
	{
		if( data_ )
			free_doubles( data_ );
		data_ = 0;
		size_ = 0;
		real_size = 0;
	}
	else
	{
		yr_error( "cVector:resize( %d )", newsize_ );
	}

}

// resize vector to newsize_ and fill with value_'s
void cVector::resize( int newsize_, double value_, int exactSize )
{
	if( !owns_data() )
		yr_error( "resize not allowed on a vector which wraps data" );

	_reallocate( newsize_, exactSize ==  1 );
	if( size_ )
	{
		if( 0.0 == value_ )
		{
			memset( data_, 0, size_ * sizeof( double ) );
		}
		else
		{
			for( double *ptr = data_, *ptr_end = data_ + size_; ptr != ptr_end; ++ptr )
				*ptr = value_;
		}
	}
}

// resize vector to newsize keep old values and fill new ones with values
void cVector::resize_keep( int newsize, double value, int exactSize )
{
	if( !owns_data() )
		yr_error( "resize not allowed on a vector which wraps data" );

	_reallocate_and_preserve( newsize, value, exactSize );
}

// resize vector to newsize_ and leave unintialised
void cVector::dirty_resize( int newsize_ )
{
	_reallocate( newsize_ );
}

// resize vector to newsize_, allocate exactly newsize_ and leave uninitialized
// if newsize_ == -1 (the default), allocate exactly newsize_ and copy the values
void cVector::exact_resize( int newsize_ )
{
	if( !owns_data() )
		yr_error( "resize not allowed on a vector which wraps data" );

	if( newsize_ == real_size ) // just change size_ the storage is correct
	{
		size_ = newsize_;
		return;
	}
	if( -1 == newsize_ )	// we just want to trim to the current size
	{
		if( 0 == size_ )
		{
			_reallocate( 0, 1 );	// Trim to nothing
		}
		else
		{
			// Trim to size_
			newsize_ = size_;
			if( newsize_ == real_size )
				return;
			double* old_p = data_;
			_reallocate( size_, 2 );
			if( newsize_ > 0 )
				memcpy( data_, old_p, newsize_ * sizeof( double ) );
			if( old_p)
				free_doubles( old_p );
		}
	}
	else
	{
		_reallocate( newsize_, 1 );
	}
}

// pre-allocate memoery for the vector to avoid resizing repeately
void cVector::reserve( int new_size )
{
	if( !owns_data() )
		yr_error( "reserve not allowed on a vector which wraps data" );

	if( real_size > new_size )
		return;
	int old_size = size_;
	double* p_old_data = data_;
	_allocate( new_size );
	if( p_old_data )
	{
		if( old_size > 0 )
			memcpy( data_, p_old_data, old_size * sizeof( double ) );
		free_doubles( p_old_data );		
	}
	size_ = old_size;
}

ostream& operator << ( ostream& outfile, const cVector& vec )
{
	for( int i = 0; i < vec.size_; ++i )
	{
		outfile << '[' << i << ']' << vec.data_[i] << ' ' ;
		if( ( i + 1 ) %10 == 0 )
			outfile << "\n    ";
	}
	outfile  << 'n';

	return outfile;
}

// operator for vector addition const version
cVector operator + ( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec );
	stackVec += rVec;
	return stackVec;
}

cVector operator + ( const cVector& lVec, double scalar )
{
	cVector stackVec( scalar, lVec.getSize() );
	VAddInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)lVec.data() );
	return stackVec;
}

cVector operator + ( double scalar, const cVector& rVec )
{
	cVector stackVec( scalar, rVec.getSize() );
	VAddInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)rVec.data() );
	return stackVec;
}

cVector operator + ( const int& scalar, const cVector& rVec )
{
	return rVec + ( double )scalar;
}

cVector operator + ( const cVector& lVec, const int& scalar )
{
	return lVec + ( double )scalar;
}

// Append operators
cVector operator && ( const cVector& lVec, const cVector& rVec )
{
	return cVector( lVec, rVec );
}

cVector operator && ( double num, const cVector& rVec )
{
	return cVector( cVector( num, 1 ), rVec );
}

cVector operator && ( const cVector& lVec, double num  )
{
	return cVector( lVec, cVector( num, 1 ) );
}

cVector& cVector::operator &= ( const cVector& rVec )
{
	int old_size = size_;
	resize( size_ + rVec.size_ );

	for( int i = 0; i < rVec.size_; ++i )
		data_[old_size++] = rVec.data_[i];

	return *this;
}

cVector& cVector::operator &= ( double num )
{
	resize( size_ + 1 );
	data_[ size_ - 1 ] = num;

	return *this;
}

// operator for subtracting
cVector operator - ( const cVector& lVec, const cVector& rVec )
{
	cVector stackVec( lVec );
	stackVec -= rVec;
	return stackVec;
}

cVector operator - ( const cVector& lVec, double scalar )
{
	cVector stackVec( -scalar, lVec.getSize() );
	VAddInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)lVec.data() );
	return stackVec;
}

cVector operator - ( double scalar, const cVector& rVec )
{
	cVector stackVec( scalar, rVec.getSize() );
	VSubInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)rVec.data() );
	return stackVec;
}

cVector operator - ( const int& scalar, const cVector& rVec )
{
	return ( double )scalar - rVec;
}

cVector operator - ( const cVector& lVec, const int& scalar )
{
	return lVec - ( double )scalar;
}

// operator for multiplication
cVector operator * ( double scalar, const cVector& rVec )
{
	cVector stackVec( scalar, rVec.getSize() );
	VMultInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)rVec.data() );
	return stackVec;
}

cVector operator * ( const cVector& lVec, double scalar )
{
	cVector stackVec( scalar, lVec.getSize() );
	VMultInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)lVec.data() );
	return stackVec;
}

cVector operator * ( const int& scalar, const cVector& rVec )
{
	return ( double )scalar * rVec;
}

cVector operator * ( const cVector& lVec, const int& scalar )
{
	return lVec * ( double )scalar;
}

cVector operator * ( const cVector& lVec, const cVector& rVec )
{
	cVector stackVec( lVec );
	stackVec *= rVec;
	return stackVec;
}

// operator for division
cVector operator / ( const cVector& lVec, const cVector& rVec )
{
	cVector stackVec( lVec );
	stackVec /= rVec;
	return stackVec;
}

cVector operator / ( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec );
	VDivInPlace( stackVec.size_, stackVec.data(), scalar );
	return stackVec;
}

cVector operator / ( double scalar, const cVector& rVec )
{
	cVector stackVec( scalar, rVec.getSize() );
	VDivInPlace( stackVec.getSize(), (double*)stackVec.data(), (double*)rVec.data() );
	return stackVec;
}

cVector operator / ( const cVector& lVec, const int& scalar )
{
	return lVec / ( double )scalar;
}

cVector operator / ( const int& scalar, const cVector& rVec )
{
	return ( double )scalar / rVec;
}


// Adds a vector to this
cVector& cVector::operator += ( const cVector& v )
{
	yr_assert( size_ == v.size_ );
	VAddInPlace( size_, data_, v.data_ );
	return *this;
}

// Subtracts a cVector from this
cVector& cVector::operator -= ( const cVector& v )
{
	yr_assert( size_ == v.size_ );
	VSubInPlace( size_, data_, v.data_ );
	return *this;
}

// Multiplies a cVector and this
cVector& cVector::operator *= ( const cVector& v )
{
	yr_assert( size_ == v.size_ );
	VMultInPlace( size_, data_, v.data_ );
	return *this;
}

// Divides a cVector from this
cVector& cVector::operator /= ( const cVector& v )
{
	yr_assert( size_ == v.size_ );
	VDivInPlace( size_, data_, v.data_ );
	return *this;
}

// Adds a double to this
cVector& cVector::operator += ( double v )
{
	VAddInPlace( size_, data_, v );
	return *this;
}

// Subtracts a double from this
cVector& cVector::operator -= ( double v )
{
	VSubInPlace( size_, data_, v );
	return *this;
}

// Multiplies a double and this
cVector& cVector::operator *= ( double v )
{
	VMultInPlace( size_, data_, v );
	return *this;
}

// Divides a double and this
cVector& cVector::operator /= ( double v )
{
	VDivInPlace( size_, data_, v );
	return *this;
}

// Adds a int to this
cVector& cVector::operator += ( int v )
{
	if ( v != 0 )
		*this += (double)v;
	return *this;
}

// Subtracts a int from this
cVector& cVector::operator -= ( int v )
{
	if ( v != 0 )
		*this -= (double)v;
	return *this;
}

// Multiplies a int from this
cVector& cVector::operator *= ( int v )
{
	if ( v != 1 )
		*this *= (double)v;
	return *this;
}

// Divides a int from this
cVector& cVector::operator /= ( int v )
{
	if ( v != 1 )
		*this /= (double)v;
	return *this;
}

cVector& cVector::Min( double x )
{
	for( double *ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ++ptr )
	{
		if( x < *ptr )
			*ptr = x;
	}
	return *this;
}

cVector& cVector::Max( double x )
{
	for( double *ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ++ptr )
	{
		if( x > *ptr )
			*ptr = x;
	}
	return *this;
}

cVector& cVector::Min( const cVector& x )
{
	yr_assert( size_ == x.size_ );
	for( double *ptr = data_, *ptr_end = data_ + size_, *x_ptr = x.data_; ptr < ptr_end; ptr++, x_ptr++ )
	{
		if( *x_ptr < *ptr )
			*ptr = *x_ptr;
	}
	return *this;
}

cVector& cVector::Max( const cVector& x )
{
	yr_assert( size_ == x.size_ );
	for( double *ptr = data_, *ptr_end = data_ + size_, *x_ptr = x.data_; ptr < ptr_end; ptr++, x_ptr++ )
	{
		if( *x_ptr > *ptr )
			*ptr = *x_ptr;
	}
	return *this;
}

cVector& cVector::Fabs()
{
	VFabsInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Exp()
{
	VExpInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Log( bool check_for_fuzz )
{
	VLogInPlace( size_, data_, check_for_fuzz );
	return *this;
}

cVector& cVector::Sin()
{
	VSinInPlace( size_, data_ );
	return *this;
}

cVector& cVector::ASin()
{
	VAsinInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Cos()
{
	VCosInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Normal()
{
	VNormalInPlace( size_, data_ );
	return *this;
}

cVector& cVector::NormalInv()
{
	VNormalInvInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Sqrt()
{
	VSqrInPlace( size_, data_ );
	return *this;
}

cVector& cVector::Pow( const cVector& v )
{
	VPowInPlace( size_, data_, (double*)v.data() );
	return *this;
}

cVector& cVector::Pow( double x )
{
	VPowInPlace( size_, data_, x );
	return *this;
}

// inverse of vector
cVector& cVector::Inverse( double factor )
{
	for( double *ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ++ptr )
		*ptr = factor / (*ptr);
	return *this;
}

cVector& cVector::AddProduct( const double multiplier, const cVector& rVec )
{
	yr_assert( rVec.size_ == size_ );
	for( double *lPtr = data_, *lPtr_end = data_ + size_, *rPtr = rVec.data_; lPtr < lPtr_end; ++lPtr, ++rPtr )
		*lPtr += multiplier * (*rPtr);

	return *this;
}

cVector& cVector::Divide( const cVector& numerator )
{
	yr_assert( size_ = numerator.size_ );
	for( double *lPtr = data_, *lPtr_end = data_ + size_, *rPtr = numerator.data_; lPtr < lPtr_end; ++lPtr, ++rPtr )
		*lPtr = yr_divide( *rPtr, *lPtr, 0.0 );
	/*
	for( int i = 0; i < size_; ++i )
		data_[i] = yr_divide( numerator.data_[i], data_[i], 0.0 );
		*/

	return *this;
}

cVector& cVector::DivideThisBy( const cVector& denominator, double backup )
{
	yr_assert( size_ = denominator.size_ );
	for( double *lPtr = data_, *lPtr_end = data_ + size_, *rPtr = denominator.data_; lPtr < lPtr_end; ++lPtr, ++rPtr )
		*lPtr = yr_divide( *lPtr, *rPtr, backup );

	return *this;
}

cVector& cVector::DivideThisBySmooth( const cVector& denominator, double backup, double tolerance )
{
	yr_assert( size_ = denominator.size_ );
	for( double *lPtr = data_, *lPtr_end = data_ + size_, *rPtr = denominator.data_; lPtr < lPtr_end; ++lPtr, ++rPtr )
		*lPtr = yr_divide_smooth( *lPtr, *rPtr, backup, tolerance );

	return *this;
}

cVector& cVector::DivideThisBy( double denominator, double backup )
{
	for( double *ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ++ptr )
		*ptr = yr_divide( *ptr, denominator, backup );

	return *this;
}

cVector cVector::Fabs( const cVector& lVec )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] > 0.0 ) ? lVec[i] : -lVec[i];

	return stackVec;
}

// Max of two vector
cVector cVector::Max( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] > rVec[i] ) ? lVec[i] : rVec[i];

	return stackVec;
}

// Min of two vector
cVector cVector::Min( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] < rVec[i] ) ? lVec[i] : rVec[i];

	return stackVec;
}

// Max of vector and scalar
cVector cVector::Max( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] > scalar ) ? lVec[i] : scalar;

	return stackVec;
}

// Max of scalar and vector
cVector cVector::Max( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( rVec[i] > scalar ) ? rVec[i] : scalar;

	return stackVec;
}

// Min of vector and scalar
cVector cVector::Min( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] < scalar ) ? lVec[i] : scalar;

	return stackVec;
}

// Min of scalar and vector
cVector cVector::Min( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( rVec[i] < scalar ) ? rVec[i] : scalar;

	return stackVec;
}

double cVector::Max() const
{
	return VMax( size_, data_ );
}

double cVector::Min() const
{
	return VMin( size_, data_ );
}

double cVector::Norm() const
{
	double sum = 0.0;
	for( double *ptr = data_, *ptr_end = data_ + size_; ptr < ptr_end; ++ptr )
		sum += ( *ptr * *ptr );
	return sqrt( sum );
}

// returns the interval that point t lies in
// starts the search from first
// interval returns n-2 if t >= data_[n-1]
int cVector::interval( double t, int adj, int first ) const
{
	int il = first;
	int ih;
	yr_assert( size_ > 0 );
	if( ( adj ) && ( t >= data_[size_-1] ) )
		return size_ - 1;

	if( ( first >= 0 ) && ( first < size_ - 1 ) )
	{
		if( ( data_[first] <= t ) && ( t < data_[first + 1] ) )
			return il;
	}
	ih = size_ - 1;

	while( ih > il + 1 )
	{
		int midpoint = (il + ih ) / 2;
		if( t < data_[midpoint] )
			ih = midpoint;
		else
			il = midpoint;
	}
	return il;
}

// returns the index of the point
// closest to t
// start the search from last
int cVector::location( double t, int first ) const
{
	int il = interval( t, 0, first );
	int ih = il + 1;
	
	if( size_ > 1 )
	{
		if( ( fabs( t - data_[il] ) ) < ( fabs( t - data_[ih] ) ) )
			return il;
		else
			return ih;
	}
	else
	{
		return il;
	}
}

int cVector::upper_bound( double t ) const
{
#ifdef NO_OBJECTSPACE
	return (int)(std::upper_bound( data_, data_ + size_, t ) - data_ );
#else
	return ::upper_bound( data_, data_ + size_, t ) - data_;
#endif
}

void cVector::Bounds( const double x, int& lower, int& upper ) const
{
	// Find position of x in the vector by bisection

	yr_assert( 0 <= lower );
	yr_assert( upper <= size_ );

	int mid = ( lower + upper ) / 2;

	while( upper - lower > 1 )
	{
		yr_assert( upper == size_ || x <= data_[upper] );
		yr_assert( data_[lower] <= x );

		if( x < data_[mid] )
		{
			upper = mid;
		}
		else
		{
			lower = mid;
		}

		mid = ( lower + upper ) / 2;
	}
}

void cVector::InsertWithoutResize( int n, double x, int k )
{
	for( int i = n; i > k; i-- )
		data_[i] = data_[i-1];
	data_[k] = x;
}

// Old interp routines from zcurve
double cVector::Interp( int& m, int n, const cVector& xv, double x, int logInterp, double& delta ) const
{
	if( n == 0 ){
		delta = 0.0;
		return 0.0;
	}

	if( n == 1 ){
		delta = 1.0;
		return data_[0];
	}

	int lower, upper, midpoint;
	double y;
	if( x <= xv[m] )
	{
		lower = m;
		upper = m + 1;
	}
	else if( x >= xv[n-1] )
	{
		lower = n - 2;
		upper = n - 1;
	}
	else
	{
		lower = m;
		upper = n - 1;
		while( upper - lower > 1 )
		{
			midpoint = ( lower + upper ) / 2;

			if( x <= xv[midpoint] )
				upper = midpoint;
			else
				lower = midpoint;
		}
	}

	if( logInterp && xv[lower] > FUZZ && x > FUZZ )
	{
		delta = log( x/xv[lower] ) / log( xv[upper] / xv[lower] );
		y = data_[lower] + delta * ( data_[upper] - data_[lower] );
	}
	else
	{
		delta = ( x - xv[lower] ) / ( xv[upper] - xv[lower] );
		y = data_[lower] + delta * ( data_[upper] - data_[lower] );
	}

	return y;
}

double cVector::Interp( int n, const cVector& xv, double x, int logInterp ) const
{
	int m = 0;
	double delta = 0.0;
	return Interp( m, n, xv, x, logInterp, delta );
}

// insert element x into posn k
void cVector::insert( int k, double x )
{
	int i;

	if( k >= size_ )
	{
		push_back( x );
	}
	else
	{
		resize( size_ + 1 );

		for( i = size_ - 1; i > k; i-- )
			data_[i] = data_[i-1];
		data_[k] = x;
	}
}

// insert element x into posn k to m
void cVector::insert( int k, int m, double x )
{
	yr_assert( m >= k );
	yr_assert( m >= 0 );	// it is m or k here? not sure
	int n = m - k + 1;
	int save_size = size_;
	resize( yr_max( n + size_, m + 1 ) );

	// shift current elements forward
	if( k < save_size )
	{
		for( int i = size_ - 1; (i-n) >= k; --i )
			data_[i] = data_[i-n];
	}
	// set new value
	for( int i = k; i <= m; ++i )
		data_[i] = x;
}

// add an element at the end (STL-like)
void cVector::push_back( double a )
{
	resize( size_ + 1 );
	data_[size_-1] = a;
}

// drop the element at posn k
void cVector::drop( int k )
{
	int i;

	// shift over to kth position
	// and resize vector
	yr_assert( k >= 0 );
	for( i = k; i < size_ - 1; ++i )
		data_[i] = data_[i+1];
	(*this).resize( size_ - 1);
}

// droop elements from posn k to n
void cVector::drop( int k, int n )
{
	int i;
	if( k > n ) //ensure that n >= k
	{
		int l = k;
		k = n;
		n = l;
	}

	// shift over to the kth position
	// and resize vector
	yr_assert( k >= 0 );
	for( i = k; i < size_ - n + k - 1; ++i )
		data_[i] = data_[i+n-k+1];

	(*this).resize( size_ - n + k - 1 );

}

// drop elements in [it_begin, it_end].
void cVector::erase( iterator it_begin, iterator it_end )
{
	if( it_begin == it_end )
		return;

	yr_assert( it_end > it_begin );
	yr_assert( it_begin >= begin() );
	yr_assert( it_end <= end() );

	while( it_end != end() )
	{
		*it_begin ++ = *it_end ++;
	}

	resize( (int)( size_ - ( it_end - it_begin ) ) );
}

void cVector::sort()
{
	yr_error("cVector::sort() not implemented yet");
}

// we assume that vector is already soted
cVector& cVector::merge( double x, double tolerance, bool replace )
{
	if( size_ == 0 )
	{
		resize( 1 );
		data_[0] = x;
		return *this;
	}

	double tol = yr_max( FUZZ, tolerance - FUZZ );

	// If the new value is before the first element of the array
	if( x < data_[0] - tol )
	{
		// prepend the new value
		int old_size = size_;
		double* old_p = data_;
		_allocate( old_size + 1 );
		data_[0] = x;
		memcpy( &data_[1], old_p, old_size * sizeof( double ) );
		free_doubles( old_p );
		return *this;
	}
	// If the new value is after the last lement of the array
	else if( x > data_[size_-1] + tol )
	{
		// Append the new value
		resize( size_ + 1 );
		data_[size_ - 1] = x;
		return *this;
	}

	// Look for the interval which straddles the value
	int lower = 0;
	int upper = size_ - 1;
	int mid;

	do
	{
		if( data_[lower] > data_[upper] )
			yr_error( "cVector::merge( double, double ); vector is not sorted!" );

		mid = ( lower + upper ) / 2;

		if( fabs( data_[lower] - x ) <= tol )
		{
			if( replace )
			{
				// in the original vector, there might be other elements within x and this one if spaced by less than tol
				// we need to replace the lower one if all are bigger than x to keep an ordered vector
				while( lower > 0 && data_[lower-1] > x )
					lower--;
				data_[lower] = yr_min( data_[lower], x );
			}
			return *this;
		}
		else if( fabs( data_[upper] - x ) <= tol )
		{
			if( replace )
			{
				// in the original vector, there might be other elements within x and this one if spaced by less than tol
				// we need to replace the lower one if all are bigger than x to keep an ordered vector
				while( upper > 0 && data_[upper-1] > x )
					upper--;
				data_[upper] = yr_min( data_[upper], x );
			}
			return *this;
		}
		else if( fabs( data_[mid] - x ) <= tol )
		{
			if( replace )
			{
				// in the original vector, there might be other elements within x and this one if spaced by less than tol
				// we need to replace the lower one if all are bigger than x to keep an ordered vector
				while( mid > 0 && data_[mid-1] > x )
					mid--;
				data_[mid] = yr_min( data_[mid], x );
			}
			return *this;
		}
		// otherwise narrow the search
		else if( x < data_[mid] )
			upper = mid;
		else if( x > data_[mid] )
			lower = mid;
		else
			yr_error( "Err.. This shouldn't happen..." );
	}
	while( ( upper - lower ) > 1 );

	double *rvec = new double[size_ - upper];
	memcpy( rvec, &data_[upper], ( size_ - upper ) * sizeof( double ) );
	resize( size_ + 1 );
	data_[lower+1] = x;
	memcpy( &data_[upper+1], rvec, ( size_ - upper - 1 ) * sizeof( double ) );
	delete [] rvec;
	return *this;
}

// this is an inclusive merge algorithm
// it will include points that are exactly tol apart
cVector& cVector::merge( const cVector& vec, double tolerance )
{
	if( vec.getSize() > 0 )
		*this = cVector( *this, vec );

	if( size_ = 0 )
		return *this;

	sort();

	double tol = yr_max( FUZZ, tolerance - FUZZ );

	int num = 0;

	for( int i = 1; i < size_; ++i )
	{
		if( data_[i] >= data_[num] + tol )
			data_[++num] = data_[i];
	}

	resize( num + 1 );
	return *this;
}

// power of vector to scalar
cVector power( const cVector& x, const double &exponent )
{
	cVector temp;
	int n_sims = x.getSize();
	if( equal( 0.0, exponent ) )
	{
		temp.resize( n_sims );
		temp.initialize( 1.0 );
	}
	else if( equal( 1.0, exponent ) )
	{
		temp = x;
	}
	else
	{
		temp.resize( n_sims );
		for( int data_ = 0; data_ < n_sims; data_++ )
		{
			temp[data_] = exp( exponent * log( fabs( x[data_] ) ) );
		}
	}
	return temp;
}

cVector pow( const cVector& in, double x )
{
	if( fabs( x - 1.0 ) < FUZZ )
		return in;

	if( fabs( x + 1.0 ) < FUZZ )
		return 1.0 / in;

	if( fabs( x ) < FUZZ )
		return cVector( 1.0, in.getSize() );

	cVector powvec( in.getSize() );

	for( int i = 0; i < in.getSize(); ++i )
		powvec[i] = pow( in[i], x );

	return powvec;
}

cVector log( const cVector& in )
{
	cVector logvec( in.getSize() );

	for( int i = 0; i < in.getSize(); ++i )
	{
		if( in[i] <= 0.0 )
			yr_error( "log( cVector ): attemping to evaluate log( %lf )", in[i] );

		logvec[i] = log( in[i] );
	}

	return logvec;
}

cVector exp( const cVector& in )
{
	cVector expvec( in );

	VExpInPlace( expvec.getSize(), expvec.data() );
	return expvec;
}

cVector asin( const cVector& in )
{
	cVector result( in );

	VAsinInPlace( result.getSize(), result.data() );
	return result;
}

cVector sin( const cVector& in )
{
	cVector result( in );

	VSinInPlace( result.getSize(), result.data() );
	return result;
}

cVector cos( const cVector& in )
{
	cVector result( in );

	VCosInPlace( result.getSize(), result.data() );
	return result;
}

cVector normal( const cVector& in )
{
	cVector result( in );

	VNormalInPlace( result.getSize(), result.data() );
	return result;
}

cVector normal_inv( const cVector& in )
{
	cVector result( in );

	VNormalInvInPlace( result.getSize(), result.data() );
	return result;
}

double mean( const cVector& in )
{
	return VMean( in.getSize(), in.data() );
}

int cVector::contains( double val, double tol ) const
{
	int i = 0;
	int count = 0;

	while( i < size_ )
	{
		if( ( val >= data_[i] - tol ) && ( val <= data_[i] + tol ) )
			count++;
		i++;
	}

	return count;
}

int cVector::find( const double val, const double tol, const int start ) const
{
	int i = start;

	while( i < size_ )
	{
		if( ( val >= data_[i] - tol ) && ( val <= data_[i] + tol ) )
			break;
		i++;
	}

	return i < size_ ? i : -1;
}


// this function returns a vector that contains adjacent points that are at most period apart
cVector sequence( int start, int end, int step )
{
	int size_  = ( end - start ) / step;

	cVector seqvec( max( size_, 1 ) );

	seqvec[0] = start;
	for( int i = 1; i < size_; ++i )
		seqvec[i] = seqvec[i-1] + step;

	return seqvec;
}

double variance( const cVector& x )
{
	return VVariance( x.getSize(), x.data() );
}

double standard_dev( const cVector& x, double& meanvalue )
{
	meanvalue = mean(x);
	return sqrt( variance( x ) );
}

// operator for vector greater than
cVector operator > ( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] > rVec[i] );

	return stackVec;
}

// operator for vector less than
cVector operator < ( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] < rVec[i] );

	return stackVec;
}

// operator for vector greater than equal
cVector operator >= ( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] >= rVec[i] );

	return stackVec;
}

// operator for vector less than equal
cVector operator <= ( const cVector& lVec, const cVector& rVec )
{
	yr_assert( lVec.getSize() == rVec.getSize() );
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] <= rVec[i] );

	return stackVec;
}

// operator for vector > const
cVector operator > ( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] > scalar );

	return stackVec;
}

// operator for vector < const
cVector operator < ( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] < scalar );

	return stackVec;
}

// operator for vector >= const
cVector operator >= ( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] >= scalar );

	return stackVec;
}

// operator for vector <= const
cVector operator <= ( const cVector& lVec, double scalar )
{
	cVector stackVec( lVec.getSize() );
	for( int i = 0; i < lVec.getSize(); ++i )
		stackVec[i] = ( lVec[i] <= scalar );

	return stackVec;
}

// operator for scalar > vector
cVector operator > ( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( scalar > rVec[i]  );

	return stackVec;
}

// operator for scalar < vector
cVector operator < ( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( scalar < rVec[i]  );

	return stackVec;
}

// operator for scalar >= vector
cVector operator >= ( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( scalar >= rVec[i]  );

	return stackVec;
}

// operator for scalar <= vector
cVector operator <= ( double scalar, const cVector& rVec )
{
	cVector stackVec( rVec.getSize() );
	for( int i = 0; i < rVec.getSize(); ++i )
		stackVec[i] = ( scalar <= rVec[i]  );

	return stackVec;
}

int cVector::NumSmaller( int n, double x ) const
{
	int count = 0;
	yr_assert( n <= size_ );
	for( int i = 0; i < n; ++i )
	{
		if( data_[i] < x )
			count++;
	}
	return count;
}

int cVector::FirstGreater( int n, double x, int first ) const
{
	yr_assert( n <= size_ && first >= 0 );
	for( int i = first; i < n; ++i )
	{
		if( data_[i] > x )
			return i;
	}
	return -1;
}

bool cVector::AllEqual( int n, double fuzz ) const
{
	if( n == 0 )	// default for n
		n = size_;
	yr_assert( n <= size_ );
	for( int i = 0; i < n; ++i )
	{
		if( fabs(data_[0] - data_[i] ) > fuzz )
			return false;
	}
	return true;
}

bool cVector::IsIncreasing( int n ) const
{
	int i;
	yr_assert( n <= size_ );
	for( i = 1; i < n; ++i )
	{
		if( data_[i-1] >= data_[i] )
			return false;
	}
	return true;
}

bool cVector::IsIncreasingNonStrict( int n ) const
{
	int i;
	yr_assert( n <= size_ );
	for( i = 1; i < n; ++i )
	{
		if( data_[i-1] - FUZZ > data_[i] )
			return false;
	}
	return true;
}

bool cVector::IsZeroVector() const
{
	for( int i = 0; i < size_; ++i )
	{
		if( fabs( data_[i] ) > FUZZ )
			return false;
	}
	return true;
}

bool cVector::IsEqual( double x ) const
{
	for( int i = 0; i < size_; ++i )
	{
		if( fabs( data_[i] - x ) > FUZZ )
			return false;
	}
	return true;
}

bool cVector::IsSameSign() const
{
	double a = 0.0;
	double b = 0.0;
	for( int i = 0; i < size_; ++i )
	{
		a += fabs( data_[i] );
		b += data_[i];
	}
	return ( fabs( a - fabs( b ) )  < FUZZ );
}

class cVectorIndexSortHelper
{
public:
	cVectorIndexSortHelper( const cVector& sVec ) : _vec( sVec )
	{
	}
	bool operator () ( int a, int b ) const
	{
		return( _vec[a] < _vec[b] );
	}

private:
	const cVector& _vec;
};

struct index_sort_elem_comp
{
	index_sort_elem_comp( const double* data ) : _data( data )
	{
	}
	bool operator () ( int a, int b ) const
	{
		return( _data[a] < _data[b] );
	}

	const double* _data;
};


cIvector index_sort( int size, const double* data )
{
	cIvector index( size );
	for( int i = 0; i < size; ++i )
		index[i] = i;

	std::sort( ( int*)index, (int*)index + size, index_sort_elem_comp( data ) );
	return index;
}



cIvector index_sort( const cVector& sVec )
{
	int n = sVec.getSize();
	cIvector index( n );
	for( int i = 0; i < n; ++i )
		index[i] = i;

	cVectorIndexSortHelper indexSortHelper( sVec );
	sort( (int*)index, (int*)index + n, indexSortHelper );
	return index;
}



cIvector stable_index_sort( const cVector& sVec )
{
	int n = sVec.getSize();
	cIvector index( n );
	for( int i = 0; i < n; ++i )
		index[i] = i;

	cVectorIndexSortHelper indexSortHelper( sVec );
	stable_sort( (int*)index, (int*)index + n, indexSortHelper );
	return index;
}



void dirty_index_sort( const cVector& sVec, cIvector& index, int n )
{
	// note that in this dirty version, the returned index size could be bigger than sVec's size
	// this is done to avoid excessive resizing when we need to sort vectors of different sizes many times
	// (e.g., in SimCopula calculations )
	if( n < 0 )
		n = sVec.getSize();
	if( index.getSize() < n )
		index.resize(n);
	for( int i = 0; i < n; ++i )
		index[i] = i;

	cVectorIndexSortHelper indexSortHelper( sVec );
	sort( (int*)index, (int*)index + n, indexSortHelper );
}



// sort a vector in-place, and then remove any elements which are within FUZZ of each other
cVector& cVector::sortunique()
{
	if( size_ > 1 )
	{
		QSort( data_, size_, 1 );
		remove_duplicates();
	}
	return *this;
}

// remove duplicates from a sorted vector
cVector& cVector::remove_duplicates( double fuzz )
{
	if( size_ > 1 )
	{
		int j = 0;
		for( int i = 1; i < size_; ++i )
		{
			if( fabs( data_[j] - data_[i] ) > fuzz )
			{
				data_[++j] = data_[i];
			}
		}
		resize( ++j );
	}
	return *this;
}

cVector yr_divide( const cVector& numerator, const cVector& denominator, const double& backup )
{
	int n = numerator.getSize();
	yr_assert( n == denominator.getSize() );
	cVector tmp( n );
	for( int i = 0; i < n; ++i )
	{
		if( fabs( denominator[i] ) < FUZZ )
		{
			tmp[i] = backup;
		}
		else
		{
			tmp[i] = numerator[i] / denominator[i];
		}
	}
	return tmp;
}

cVector yr_divide( const cVector& numerator, const double& denominator, const double& backup )
{
	int n = numerator.getSize();
	cVector tmp( n );
	for( int i = 0; i < n; ++i )
	{
		if( fabs( denominator ) < FUZZ )
		{
			tmp[i] = backup;
		}
		else
		{
			tmp[i] = numerator[i] / denominator;
		}
	}
	return tmp;
}

cVector yr_divide( const double& numerator, const cVector& denominator, const double& backup )
{
	int n = denominator.getSize();
	cVector tmp( n );
	for( int i = 0; i < n; ++i )
	{
		if( fabs( denominator[i] ) < FUZZ )
		{
			tmp[i] = backup;
		}
		else
		{
			tmp[i] = numerator / denominator[i];
		}
	}
	return tmp;
}

/*
// Helps to resolve platform differences by taking the minimum of sort_index[m], for which
// fabs( vec[ sort_index[start_index] ] - vec[sort_index[m]] ) <= FUZZ.
int minimize_sort_index( const cVector& vec, const cIvector& sort_index, int start_index )
{
	int min_sort_index = sort_index[start_index];
	double guess = vec[min_sort_index];
	int i;
	for( i = start_index + 1; i < sort_index.getSize(); ++i )
	{
		if( equal( guess, vec[sort_index[i]] ) )
			min_sort_index = yr_min( min_sort_index, sort_index[i] );
		else
			break;
	}
	for( i = start_index - 1; i >= 0; --i )
	{
		if( equal( guess, vec[sort_index[i]] ) )
			min_sort_index = yr_min( min_sort_index, sort_index[i] );
		else
			break;
	}
	return min_sort_index;
}
*/

cVector subvector( const cVector& a, int from, int to )
{
	const int new_size = max( to - from, 0 ) + 1;
	cVector temp(new_size);

	for( int i = 0; i < new_size; ++i )
		temp[i] = a[from + i];

	return temp;
}

// double* utilities which can be used buffer of doubles
inline void VAddInPlaceDefault( int size, double* data, double* other )
{
	for( double *ptr = data, *ptr_end = data + size, *ptr_v = other; ptr < ptr_end; ++ptr, ++ptr_v )
		*ptr += *ptr_v;
}

void VAddInPlace( int size, double* data, double* other )
{
	VAddInPlaceDefault( size, data, other );
}

inline void VAddInPlaceDefault( int size, double* data, double v )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr += v;
}

void VAddInPlace( int size, double* data, double v )
{
	VAddInPlaceDefault( size, data, v );
}

// subtract other from data in place
inline void VSubInPlaceDefault( int size, double* data, double* other )
{
	for( double *ptr = data, *ptr_end = data + size, *ptr_v = other; ptr < ptr_end; ++ptr, ++ptr_v )
		*ptr -= *ptr_v;
}

void VSubInPlace( int size, double* data, double* other )
{
	VSubInPlaceDefault( size, data, other );
}

inline void VSubInPlaceDefault( int size, double* data, double v )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr -= v;
}

void VSubInPlace( int size, double* data, double v )
{
	VSubInPlaceDefault( size, data, v );
}

// multiply data by other in place
inline void VMultInPlaceDefault( int size, double* data, double* other )
{
	for( double* ptr = data, *ptr_end = data + size, *ptr_v = other; ptr < ptr_end; ++ptr, ++ptr_v )
		*ptr *= *ptr_v;
}

void VMultInPlace( int size, double* data, double* other )
{
	VMultInPlaceDefault( size, data, other );
}

inline void VMultInPlaceDefault( int size, double* data, double v )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr *= v;
}

void VMultInPlace( int size, double* data, double v )
{
	VMultInPlaceDefault( size, data, v );
}

// divide data by other in place
inline void VDivInPlaceDefault( int size, double* data, double* other )
{
	for( double* ptr = data, *ptr_end = data + size, *ptr_v = other; ptr < ptr_end; ++ptr, ++ptr_v )
		*ptr /= *ptr_v;
}

void VDivInPlace( int size, double* data, double* other )
{
	VDivInPlaceDefault( size, data, other );
}

inline void VDivInPlaceDefault( int size, double* data, double v )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr /= v;
}

void VDivInPlace( int size, double* data, double v )
{
	VDivInPlaceDefault( size, data, v );
}

// power
inline void VPowInPlaceDefault( int size, double* data, double* other )
{
	for( double* ptr = data, *ptr_end = data + size, *ptr_v = other; ptr < ptr_end; ++ptr, ++ptr_v )
		*ptr = pow( *ptr,*ptr_v );
}

void VPowInPlace( int size, double* data, double* other )
{
	VPowInPlaceDefault( size, data, other );
}

inline void VPowInPlaceDefault( int size, double* data, double v )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = pow( *ptr, v );
}

void VPowInPlace( int size, double* data, double v )
{
	VPowInPlaceDefault( size, data, v );
}

// fabs in place
inline void VFabsInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
	{
		if( *ptr < 0.0 )
			*ptr = -(*ptr);
	}
}

void VFabsInPlace( int size, double *data )
{
	VFabsInPlaceDefault( size, data );
}

// exp in place
inline void VExpInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = exp(*ptr);
}

void VExpInPlace( int size, double *data )
{
	VExpInPlaceDefault( size, data );
}

// log in place
inline void VLogInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = yr_log(*ptr);
}

void VLogInPlace( int size, double *data, bool check_for_fuzz )
{
	VLogInPlaceDefault( size, data );
}

// asin in place
inline void VAsinInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = asin(*ptr);
}

void VAsinInPlace( int size, double *data )
{
	VAsinInPlaceDefault( size, data );
}

// sin in place
inline void VSinInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = sin(*ptr);
}

void VSinInPlace( int size, double *data )
{
	VSinInPlaceDefault( size, data );
}

// cos in place
inline void VCosInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = cos(*ptr);
}

void VCosInPlace( int size, double *data )
{
	VCosInPlaceDefault( size, data );
}

// normal in place
inline void VNormalInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = normal(*ptr);
}

void VNormalInPlace( int size, double *data )
{
	VNormalInPlaceDefault( size, data );
}

// normal inverse in place
inline void VNormalInvInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = normal_inv2(*ptr);
}

void VNormalInvInPlace( int size, double *data )
{
	VNormalInvInPlaceDefault( size, data );
}

// sqrt in place
inline void VSqrtInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = sqrt(*ptr);
}

void VSqrtInPlace( int size, double *data )
{
	VSqrtInPlaceDefault( size, data );
}

// sqr in place
inline void VSqrInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = sqr(*ptr);
}

void VSqrInPlace( int size, double *data )
{
	VSqrInPlaceDefault( size, data );
}

// 1/x in place
inline void VInvInPlaceDefault( int size, double *data )
{
	for( double *ptr = data, *ptr_end = data + size; ptr < ptr_end; ++ptr )
		*ptr = 1.0 / (*ptr);
}

void VInvInPlace( int size, double *data )
{
	VInvInPlaceDefault( size, data );
}

inline double VSumProdDefault( size_t size, const double* lv, const double* rv )
{
	double result = 0.0;
	for( size_t i = 0; i < size; ++i, ++lv, ++rv )
		result += (*lv) * (*rv );
	return result;
}

double VSumProd( size_t size, const double* lv, const double* rv )
{
	return VSumProdDefault( size, lv, rv );
}

// compute y = ax + y
inline void VScaledAddDefault( size_t size, double a, const double* x, double* y )
{
	for( size_t i = 0; i < size; ++i, ++x, ++y )
		*y += a * *x;
}

void VScaledAdd( size_t size, double a, const double* x, double* y )
{
	VScaledAddDefault( size, a, x, y );

}

inline cVector SqrtDefault( const cVector& vec )
{
	int i, n = vec.getSize();
	cVector tempvec(n);

	for( i = 0; i < n; ++i )
		tempvec[i] = sqrt( fabs(vec[i] ) );
	
	return tempvec;
}

cVector sqrt( const cVector& vec )
{
	return SqrtDefault( vec );
}

double VMax( int size, const double* data )
{
	if( size == 0 )
		return 0.0;

	double largest = data[0];
	for( const double* ptr = data + 1, *ptr_end = data + size; ptr < ptr_end; ++ptr )
	{
		if( *ptr > largest )
			largest = *ptr;
	}
	return largest;
}


double VMin( int size, const double* data )
{
	if( size == 0 )
		return 0.0;

	double smallest = data[0];
	for( const double* ptr = data + 1, *ptr_end = data + size; ptr < ptr_end; ++ptr )
	{
		if( *ptr < smallest )
			smallest = *ptr;
	}
	return smallest;
}

double VMean( int size, const double* data )
{
	if( size == 0 )
		return 0.0;

	return VSum( size, data ) / size;

}

double VVariance( int size, const double* data )
{
	double varx = 0.0;

	if( size == 0 )
		yr_error( "variance( cVector ): vector has zero size!" );

	double mean_x = VMean( size, data );
	double dumx;

	for( int i = 0; i < size; ++i )
	{
		dumx = data[i] - mean_x;
		varx += dumx * dumx;
	}

	return varx / size;
}

const cVector theVector = cVector();
