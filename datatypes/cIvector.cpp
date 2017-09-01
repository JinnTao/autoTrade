#include <math_defs.h>
#include <cIvector.h>
#include <cSort.h>
#include <yr_assert.h>

void cIvector::_allocate()
{
	if( size_ > 0 )
	{
		data_ = new int[ size_ ];
		if( data_ == NULL )
			yr_error( "cIvector: No more memoery for Ivector objects!" );
	}
	else
		data_ = NULL;
}

cIvector::cIvector( int m )
: size_( m )
{
	_allocate();
	memset( data_, 0, size_ * sizeof( int ) );
}

cIvector::cIvector( int value, int size )
: size_( size )
{
	_allocate();
	for( int i = 0; i < size_; ++i )
		data_[i] = value;
}

cIvector::cIvector( const cIvector& vec )
: size_( vec.size_ )
{
	yr_assert( this != & vec );
	_allocate();
	memcpy( data_, vec.data_, size_ * sizeof( int ) );
}

cIvector::cIvector( const cIvector& lVec, const cIvector& rVec )
: size_( lVec.size_ + rVec.size_ )
{
	yr_assert( this != &lVec && this != &rVec );
	_allocate();
	memcpy( data_, lVec.data_, lVec.size_ * sizeof( int ) );
	memcpy( data_ + lVec.size_, rVec.data_, rVec.size_ * sizeof( int ) );
}

cIvector::~cIvector()
{
#ifdef _DEBUG
	if( data_ == ( int* ) - 1 )
		yr_error( "Object already deleted!" );
#endif

	if( data_ )
		delete [] data_;

#ifdef _DEBUG
	data_ = ( int*) - 1;
	size_ = -1;
#endif
}

cIvector& cIvector::operator = ( const cIvector& vec )
{
	if( &vec != this )
	{
		if( vec.size_ )
		{
			if( size_ != vec.size_ )
			{
				size_ = vec.size_;
				if( data_ )
					delete [] data_;
				_allocate();
			}
			memcpy( data_, vec.data_, size_ * sizeof( int ) );
		}
		else
		{
			if( data_ )
				delete [] data_;
			data_ = NULL;
			size_ = 0;
		}
	}

	return *this;
}

void cIvector::initialize( int val )
{
	for( int i = 0; i < size_; ++i )
		data_[i] = val;
}

bool cIvector::operator == ( const cIvector& vec ) const
{
	if( this == &vec )
		return true;
	if( vec.size_ != size_ )
		return false;

	for( int i = 0; i < vec.size_; ++i )
	{
		if( vec[i] != data_[i] )
			return false;
	}
	return true;
}

cIvector cIvector::operator == ( int a ) const
{
	cIvector tmp( 0, size_ );
	for( int i = 0; i < size_; ++i )
	{
		if( data_[i] == a )
			tmp[i] = 1;
	}
	return tmp;
}

void cIvector::resize( int newsize_, int value_ )
{
	if( newsize_ > 0 )
	{
		if( newsize_ != size_ )
		{
			size_ = newsize_;
			if( data_ )
				delete [] data_;
			_allocate();
		}

		if( value_ == 0 )
			memset( data_, 0, size_ * sizeof( int ) );
		else
		{
			for( int i = 0; i < size_; ++i )
				data_[i] = value_;
		}
	}
	else if( newsize_ < 0 )
		yr_error( "cIvector::resize( %d, value )", newsize_ );
	else
	{
		if( data_ )
			delete [] data_;
		data_ = NULL;
		size_ = 0;
	}
}

void cIvector::resize( int newsize_ )
{
	if( newsize_ == size_ )
		return;

	if( newsize_ > 0 )
	{
		int* old_p = data_;
		int old_size = size_;
		size_ = newsize_;
		_allocate();
		if( size_ > old_size )
		{
			if( old_size > 0 )
				memcpy( data_, old_p, old_size * sizeof( int ) );
			if( size_ > old_size )
				memset( data_ + old_size, 0, ( size_ - old_size ) * sizeof( int ) );
		}
		else
			memcpy( data_, old_p, size_ * sizeof( int ) );

		if( old_p )
			delete [] old_p;
	}
	else if( newsize_ < 0 )
		yr_error( "cIvector::resize( %d )", newsize_ );
	else
	{
		if( data_ ) delete [] data_;
		data_ = NULL;
		size_ = 0;
	}
}

int cIvector::Max() const
{
	int largest = 0;
	if( size_ )
	{
		largest = data_[0];
		for( int i = 0; i < size_; ++i )
		{
			if( data_[i] > largest )
				largest = data_[i];
		}
	}
	return largest;
}

int cIvector::Min() const
{
	int smallest = 0;
	if( size_ )
	{
		smallest = data_[0];
		for( int i = 0; i < size_; ++i )
		{
			if( data_[i] < smallest )
				smallest = data_[i];
		}
	}
	return smallest;
}

bool cIvector::AllEqual( const int i ) const
{
	return ( this->Max() == i && this->Min() == i );
}

int cIvector::Sum() const
{
	int sum = 0;
	for( int i = 0; i < size_; ++i )
		sum += data_[i];
	return sum;
}

int cIvector::AbsSum() const
{
	int sum = 0;
	for( int i = 0; i < size_; ++i )
		sum += abs( data_[i] );
	return sum;
}

int cIvector::NotEqual( const int n, const int val ) const
{
	int count = 0;
	for( int i = 0; i < n; ++i )
	{
		if( data_[i] != val )
			count++;
	}
	return count;
}

void cIvector::InsertWithoutResize( int n, int x, int k )
{
	for( int i = n; i > k; --i )
		data_[i] = data_[i-1];
	data_[k] = x;
}

void cIvector::insert( int k, int x )
{
	int i;

	if( k >= size_ )
	{
		push_back( x );
	}
	else
	{
		resize( size_ + 1 );

		for( i = size_ - 1; i > k; --i )
			data_[i] = data_[i-1];
		data_[k] = x;
	}
}

void cIvector::drop( int index, int howmany /*= 1*/ )
{
	if( howmany == 0 )
		return;

	if( index < 0 || index >= size_ )
		yr_error( "Invalid index in cIvector::drop!" );

	for( int i = index; i < size_ - howmany; ++i )
	{
		data_[i] = data_[i+howmany];
	}

	resize( yr_max( size_ - howmany, 0 ) );
}

void cIvector::push_back( const int value )
{
	resize( size_ + 1 );

	data_[size_ - 1 ] = value;
}

// returns the interval that point t lies in
// starts the search from last
// interval returns n-2 if t >= p[n-1]
int cIvector::interval( int t, int adj, int last ) const
{
	int il = last;
	int ih;
	yr_assert( size_ > 0 );
	if( (adj) && ( t >= data_[size_-1] ) )
		return size_ - 1;

	if( ( last >= 0) && ( last < size_ - 1 ) )
	{
		if( ( data_[last] <= t ) && ( t < data_[last + 1 ] ) )
			return il;
	}
	ih = size_ - 1;

	while( ih > il + 1 )
	{
		int midpoint = ( il + ih ) / 2;
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
int cIvector::location( int t, int last )
{
	int il = interval( t, 0, last );
	int ih = il + 1;

	if( size_ == 1 )
		return il;

	if( ( fabs((double)t-data_[il]) ) < ( fabs( (double)t - data_[ih] ) ) )
		return il;
	else
		return ih;
}

bool cIvector::IsIncreasing() const
{
	for( int i = 0; i < size_ - 1; ++i )
	{
		if( data_[i] >= data_[i+1] )
			return false;
	}
	return true;
}

bool cIvector::IsIncreasingNonStrict() const
{
	for( int i = 1; i < size_; ++i )
	{
		if( data_[i-1] > data_[i] )
			return false;
	}
	return true;
}

cIvector operator && ( const cIvector& lVec, const cIvector& rVec )
{
	return cIvector( lVec, rVec );
}

cIvector& cIvector::operator &= ( const cIvector& rVec )
{
	*this = cIvector( *this, rVec );
	return *this;
}

cIvector& cIvector::operator &= ( int lv )
{
	*this = cIvector( *this, cIvector( lv, 1 ) );
	return *this;
}

cIvector merge( const cIvector& a1, const cIvector& a2 )
{
	return cIvector( a1, a2 ).sortunique();
}

int FindIndex( const cIvector& a, const int& b )
{
	for( int i = 0; i < a.getSize(); ++i )
	{
		if( equal( a[i], b ) )
			return i;
	}
	return -1;
}

class cIvectorIndexSortHelper
{
public:
	cIvectorIndexSortHelper( const cIvector& sVec ) : _vec( sVec )
	{
	}
	int operator() ( int a, int b ) const
	{
		return ( _vec[a] < _vec[b] );
	}

private:
	const cIvector& _vec;

};

cIvector index_sort( const cIvector& sVec )
{
	int n = sVec.getSize();
	cIvector index( n );

	for( int i = 0; i < n; ++i )
		index[i] = i;

	cIvectorIndexSortHelper  indexSortHelper( sVec );
	sort( (int*)index, (int*)index + n, indexSortHelper );
	return index;
}

cIvector stable_index_sort( const cIvector& sVec )
{
	int n = sVec.getSize();
	cIvector index( n );

	for( int i = 0; i < n; ++i )
		index[i] = i;

	cIvectorIndexSortHelper indexSortHelper( sVec );
	stable_sort( (int*)index, (int*)index + n, indexSortHelper );
	return index;
}

cIvector subvector( const cIvector& a, int from, int to )
{
	int new_size = max( to - from, 0 ) + 1;
	cIvector temp(new_size);

	for( int i = 0; i < new_size; ++i )
	{
		temp[i] = a[from+i];
	}
	return temp;
}

//
// sort a vector in-place and then remove any elements
// which are the same as each other
//
cIvector& cIvector::sortunique()
{
	if( size_ > 1 )
	{
		QSort( data_, size_, 1 );

		int j = 0;
		for( int i = 1; i < size_; ++i )
		{
			if( data_[j] != data_[i] )
			{
				data_[++j] = data_[i];
			}
		}
		resize(++j);
	}
	return *this;
}

// output operator
ostream& operator << ( ostream& outfile, const cIvector& vec )
{
	for( int i = 0; i < vec.size_; ++i )
	{
		outfile << "[" << i << "]" << vec.data_[i] << " ";
		if( ( i + 1 ) % 10 == 0 )
			outfile << "\n";
	}
	outfile << "\n";

	return outfile;
}

int cIvector::BinarySum() const
{
	int flags = 0;
	for( int i = 0; i < size_; ++i )
	{
		if( data_[i] )
			flags++;
	}
	return flags;
}

const cIvector theVector = cIvector();


