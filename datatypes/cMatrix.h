/***
*cVector.h - declarations/definitions for matrix class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __CMATRIX_H__
#define __CMATRIX_H__

#include <stl.h>
#include <assert.h>
#include <yr_assert.h>
#include <math_defs.h>
#include <cSharedPtr.h>
#include <cVector.h>

#pragma warning ( disable:4996 )


template< typename T >
class cMatrix
{
public:
	typedef typename std::vector< T >::iterator iterator;
	typedef typename std::vector< T >::const_iterator const_iterator;
	typedef cSharedPtr< std::vector< T > > cSharedVectorPtr;

	explicit cMatrix( size_t nrows = 0, size_t ncols = 0, T def = T() )
		: m_nrows( nrows )
		, m_ncols( ncols )
		, m_p_data( new std::vector< T >( m_nrows * m_ncols, def ) )
	{
		SetRowPointers();
		SetOffSetsToCurrentSize();
	}


	explicit cMatrix( const cMatrix< T >& other, size_t start_row, size_t nrows )
		: m_nrows( nrows)
		, m_ncols( other.m_ncols )
		, m_p_data( other.m_p_data )
	{
		YR_ASSERT( start_row + nrows <= other.m_nrows, "New cMatrix's size doesn't match the source" );
		if( m_p_data->empty() )
		{
			m_row_pointers.clear();
			m_offset_beg = 0;
			m_offset_end = 0;
		}
		else
		{
			m_row_pointers.resize( m_nrows );
			for( size_t i = 0; i < m_nrows; ++i )
				m_row_pointers[i] = other.m_row_pointers[i + start_row];
			m_offset_beg = other.m_offset_beg + start_row * m_ncols;
			m_offset_end = m_offset_beg + nrows * m_ncols;
		}
	}


	cMatrix( const cMatrix< T >& other )
		: m_nrows( other.m_nrows )
		, m_ncols( other.m_ncols )
		, m_p_data( other.m_p_data )
		, m_row_pointers( other.m_row_pointers )
		, m_offset_beg( other.m_offset_beg )
		, m_offset_end( other.m_offset_end )
	{
		//SetRowPointers();
	}

	~cMatrix()
	{
	}

	
	void resize( size_t nrows, size_t ncols, T def = T() )
	{
		if( ( m_nrows == nrows ) && ( m_ncols == ncols ) )
			return;

		cSharedVectorPtr p_new_data = cSharedVectorPtr( new std::vector< T >( nrows * ncols, def ) );

		// Copy the contents, so we preserve contents on resize
		size_t min_rows = nrows;
		if( min_rows > m_nrows )
			min_rows = m_nrows;
		size_t min_cols = ncols;
		if( min_cols > m_ncols )
			min_cols = m_ncols;

		for( size_t r = 0; r < min_rows; ++r )
			std::copy( row_begin(r), row_begin(r) + min_cols, p_new_data->begin() + r*ncols );

		// Change the contents
		m_p_data = p_new_data;
		m_nrows = nrows;
		m_ncols = ncols;
		SetOffSetsToCurrentSize();
		SetRowPointers();
	}

	const cMatrix< T >& operator = ( const cMatrix< T >& other )
	{
		if( this == &other )
			return *this;

		m_nrows = other.m_nrows;
		m_ncols = other.m_ncols;
		m_p_data = other.m_p_data;
		
		m_row_pointers = other.m_row_pointers;
		m_offset_beg = other.m_offset_beg;
		m_offset_end = other.m_offset_beg_end;
	}
	
	bool operator == ( const cMatrix< T >& other ) const
	{
		return std::equal( begin(), end(), other.begin() );
	}

	inline size_t nrows() const
	{
		return m_nrows;
	}

	inline size_t ncols() const
	{
		return m_ncols;
	}

	inline T& get_item( size_t i )
	{
		yr_assert( i < m_p_data->size() );
		CopyOnWrite();
		return *(begin() + i );
	}

	inline const T& get_item( size_t i ) const
	{
		yr_assert( i < m_p_data->size() );
		return *(begin() + i );
	}

	inline T& operator() ( size_t irow, size_t icol )
	{
		yr_assert( irow < m_nrows );
		yr_assert( icol < m_ncols );
		CopyOnWrite();
		return m_row_pointers[irow][icol];
	}

	inline const T& operator() ( size_t irow, size_t icol ) const
	{
		yr_assert( irow < m_nrows );
		yr_assert( icol < m_ncols );
		return m_row_pointers[irow][icol];
	}

	// same as operator(i,j), just keeps those compilers which are not happy with this->operator()(i,j)
	inline T& get( size_t irow, size_t icol )
	{
		yr_assert( irow < m_nrows );
		yr_assert( icol < m_ncols );
		CopyOnWrite();
		return m_row_pointers[irow][icol];
	}

	inline const T& get( size_t irow, size_t icol ) const
	{
		yr_assert( irow < m_nrows );
		yr_assert( icol < m_ncols );
		return m_row_pointers[irow][icol];
	}

	// note: checks bounds even in release build
	inline T& at( size_t irow, size_t icol )
	{
		YY_ASSERT( irow < m_nrows, "matrix index out of bounds" );
		YY_ASSERT( icol < m_ncols, "matrix index out of bounds" );
		CopyOnWrite();
		return m_row_pointers[irow][icol];
	}

	inline const T& at( size_t irow, size_t icol ) const
	{
		YY_ASSERT( irow < m_nrows, "matrix index out of bounds" );
		YY_ASSERT( icol < m_ncols, "matrix index out of bounds" );
		return m_row_pointers[irow][icol];
	}

	inline T* operator[]( size_t irow )
	{
		yr_assert( irow < m_nrows );
		CopyOnWrite();
		return m_row_pointers[irow];
	}

	inline const T* operator[]( size_t irow ) const
	{
		yr_assert( irow < m_nrows );
		return m_row_pointers[irow];
	}

	iterator begin()
	{
		CopyOnWrite();
		return m_p_data->begin() + m_offset_beg;
	}

	const_iterator begin() const
	{
		return m_p_data->begin() + m_offset_beg;
	}

	iterator end()
	{
		CopyOnWrite();
		return m_p_data->begin() + m_offset_end;
	}

	const_iterator end() const
	{
		return m_p_data->begin() + m_offset_end;
	}

	iterator row_begin( size_t i_row )
	{
		YR_ASSERT( i_row < m_nrows, "matrix index out of bounds" );
		CopyOnWrite();
		return begin() + i_row * m_ncols;
	}

	const_iterator row_begin( size_t i_row ) const
	{
		YR_ASSERT( irow < m_nrows, "matrix index out of bounds" );
		return begin() + i_row * m_ncols;
	}

	iterator row_end( size_t i_row )
	{
		YR_ASSERT( irow < m_nrows, "matrix index out of bounds" );
		CopyOnWrite();
		return begin() + ( i_row + 1 )* m_ncols;
	}

	const_iterator row_end( size_t i_row ) const
	{
		YR_ASSERT( irow < m_nrows, "matrix index out of bounds" );
		return begin() + ( i_row + 1 ) * m_ncols;
	}

	const std::vector< T >& as_vector() const
	{
		CopyOnWrite();
		return *m_p_data;
	}

	size_t size() const
	{
		return m_nrows*m_ncols;
	}
	
	size_t Rows() const
	{
		return m_nrows;
	}

	size_t Cols() const
	{
		return m_ncols;
	}

	void DropRows( const vector< size_t >& rows, bool validate );
	void DropCols( const vector< size_t >& cols, bool validate );
	void IdentityMatrix();
	bool isIdentity() const;
	cMatrix< T > inverse( double inverseEpsilon = 0.000001 ) const;
	void Product( const cMatrix< T >& l, const cMatrix< T >& r );
	void transpose( cMatrix< T >& output ) const;
	cMatrix< T > transpose() const;

private:
	mutable size_t m_nrows;
	mutable size_t m_ncols;
	//std::vector< T > m_data;
	mutable cSharedVectorPtr m_p_data;
	mutable std::vector< T* > m_row_pointers;
	mutable size_t m_offset_beg, m_offset_end;

	void SetRowPointers() const;
	void CopyOnWrite() const;
	void SetOffSetsToCurrentSize() const
	{
		m_offset_beg = 0;
		m_offset_end = m_nrows * m_ncols;
	}
};

template< class T >
void cMatrix< T >::SetRowPointers() const
{
	if( m_p_data->empty() )
	{
		m_row_pointers.clear();
	}
	else
	{
		m_row_pointers.resize( m_nrows );
		T* p0 = &((*m_p_data)[0]);
		for( size_t i = 0; i < m_nrows; ++i )
			m_row_pointers[i] = p0 + i*m_ncols;
	}
}


template< class T >
void cMatrix< T >::DropRows( const vector< size_t >& rows, bool validate )
{
	typedef vector< size_t > arg_type;
	CopyOnWrite();

	vector< bool > keep_rows( m_nrows, true );
	// validate rows to be dropped

	arg_type::const_iterator iter = rows.begin();
	for( ; iter != rows.end(); ++iter )
	{
		if( validate )
			YR_ASSERT( *iter < m_nrows, "invalid row: " << *iter );
		keep_rows[*iter] = false;
	}

	size_t new_nrows = m_nrows - rows.size();
	cSharedVectorPtr p_new_data = cSharedVectorPtr( new std::vector< T >( new_nrows * m_ncols ) );

	size_t irow = 0;
	for( size_t r = 0; r < m_nrows; ++r )
	{
		if( keep_rows[r] )
		{
			std::copy( begin() + r*m_ncols, begin() + r*m_ncols + m_ncols, &((*p_new_data)[irow*m_ncols]) );
			++irow;
		}
	}

	m_p_data = p_new_data;
	m_nrows = new_nrows;
	SetRowPointers();
}


template< class T >
void cMatrix< T >::DropCols( const vector< size_t >& cols, bool validate )
{
	typedef vector< size_t > arg_type;

	vector< bool > keep_cols( m_ncols, true );

	// validate cols to be droped

	arg_type::const_iterator iter = cols.begin();
	for( ; iter != cols.end(); ++iter )
	{
		if( validate )
			YR_ASSERT( *iter < m_ncols, "invalid col : " << *iter );
		keep_cols[*iter] = false;
	}

	size_t new_ncols = m_ncols - cols.size();
	cSharedVectorPtr p_new_data = cSharedVectorPtr( new std::vector<T>( m_nrows*new_ncols ) );
	size_t icol = 0;
	for( size_t c = 0; c < m_ncols; ++c )
	{
		if( keep_cols[c] )
		{
			for( size_t r = 0; r < m_nrows; ++r )
			{
				(*p_new_data)[r * new_ncols + icol] = m_row_pointers[r][c];
			}
			++icol;
		}
	}

	m_p_data = p_new_data;
	m_ncols = new_ncols;
	SetRowPointers();
	SetOffsetsToCurrentSize();
}

template< class T >
void cMatrix< T >::IdentityMatrix()
{
	CopyOnWrite();
	int rows = Rows();
	int cols = Cols();

	int i, j;
	if( rows == cols )
	{
		for( i = 0; i < rows; ++i )
		{
			for ( j = 0; j < cols; ++j )
			{
				get( i, j ) = ( double ) ( i == j );
			}
		}
	}
	else
	{
		std::fill( begin(), end(), 0.0 );
	}
}

template< class T >
cMatrix< T > cMatrix< T >::inverse( double inverseEpsilon ) const
{
	int rows = Rows();
	int cols = Cols();

	if( rows != cols )
		return *this;

	cMatrix< T > B( rows, rows );
	//
	// A starts as a copy of self and B as the identity matrix
	// A and B are swapped over using a series of row transformations
	// In the event of a sigularity, the unmodifed self is returned
	//
	int i, j, k, n;
	double p;
	cMatrix< T > A( *this );
	B.IdentityMatrix();
	for( i = 0; i < rows; ++i )
	{
		n = 0;
		p = -1.0;

		for( j = i; j < rows; ++j )
		{
			//
			// The largest element in the column will be used as the pivot
			//
			if( fabs( A( j, i ) ) > p )
			{
				n = j;
				p = fabs( A( j, i ) );
			}
		}
		if( p > inverseEpsilon )
		{
			for( j = 0; j < cols; ++j )
			{
				yr_swap( A( i,j ), A( n,j ) );
				yr_swap( B( i,j ), B( n,j ) );
			}
			p = A( i, i );
			for( j = 0; j < cols; ++j )
			{
				A( i,j ) /= p;
				B( i,j ) /= p;
			}
			for( j = 0; j < cols; ++j )
			{
				if( i == j )
					continue;

				p = A( j, i );
				for( k = 0; k < cols; ++k )
				{
					A( j, k ) -= p * A( i, k );
					B( j, k ) -= p * B( i, k );
				}
			}
		}
		else
		{
			// Singular matrix
			B.IdentityMatrix();
			break;
		}
	}
	return B;
}

template< class T >
bool cMatrix< T >::isIdentity() const
{
	int rows = nrows();
	int cols = ncols();
	if( rows != cols )
		return false;

	for( int i = 0; i < rows; ++i )
	{
		for( int j = 0; j < cols; ++j )
		{
			if( get( i,j ) != ( double )( i == j ) )
				return false;
		}
	}

	return true;
}

template< class T >
void cMatrix< T >::Product( const cMatrix< T >& l, const cMatrix< T >& r )
{
	ProductInPlace(*this, l, r );
}

template< class T >
void cMatrix< T >::CopyOnWrite() const
{
	if( m_p_data.Unique() )
		return;

	cSharedVectorPtr p_new_data( new std::vector< T >( begin(), end() ) );
	m_p_data.Swap( p_new_data );
	SetRowPointers();
	SetOffSetsToCurrentSize();
}

template< class T >
void transpose( const cMatrix< T >& input, cMatrix< T >& output )
{
	size_t cols = input.ncols();
	size_t rows = input.nrows();
	output.resize( cols, rows );
	for( size_t j = 0; j < rows; ++j )
	{
		for( size_t i = 0; i < cols; ++i )
		{
			output(i,j) = input(j,i);
		}
	}
}

template< class T >
void cMatrix< T >::transpose( cMatrix< T >& output ) const
{
	::transpose( *this, output );
	return output;
}

// Matrix - Matrix product
template< class T >
void ProductInPlace( cMatrix< T >&m, const cMatrix< T >& l, const cMatrix< T >& r )
{
	size_t lCols = l.ncols();
	size_t rCols = r.ncols();
	size_t lRows = l.nrows();
	size_t rRows = r.nrows();
	yr_assert( lCols == rRows );

	m.resize( lRows, rCols );
	for( size_t i = 0; i < lRows; ++i )
	{
		for( size_t j = 0; j < rCols; ++j )
		{
			for( size_t k = 0; k < lCols; ++k )
				m(i,j) += l(i,k) * r(k,j);
		}
	}
}

// Matrix - vector product
template< class T >
void ProductInPlace( cMatrix< T >& m, const cMatrix< T >& l, const std::vector< T >& r )
{
	size_t l_cols = l.ncols();
	size_t l_rows = l.nrows();
	size_t r_size = r.size();

	yr_assert( l_cols == r_size );

	m.resize( l_rows, 1 );
	for( size_t i = 0; i < l_rows; ++i )
	{
		for( size_t k = 0; k < l_cols; ++k )
		{
			m(i,0) += l(i,k) * r[k];
		}
	}
}

// vector - Matrix product
template< class T >
void ProductInPlace( cMatrix< T >& m, const std::vector< T >& l, const cMatrix< T >& r )
{
	size_t l_size = l.size();
	size_t r_cols = r.ncols();
	size_t r_rows = r.nrows();

	yr_assert( l_size == r_rows );

	m.resize( l, r_cols );

	for( size_t j = 0; j < r_cols; ++j )
	{
		for( size_t k = 0; k < l_size; ++k )
			m(0,j) += l[k] * r(k,j);
	}
}

void ProductInPlaceEx( cMatrix< double >& m,
					  const cMatrix< double >& l,
					  const cMatrix< double> & r,
					  bool use_mkl = false );

void ProductInPlaceEx( cMatrix< double >& m,
					  const cMatrix< double >& lm,
					  const std::vector< double> & rv,
					  bool use_mkl = false );

void ProductInPlaceEx( cMatrix< double >& m,
					  const std::vector< double> & lv,
					  const cMatrix< double >& rm,
					  bool use_mkl = false );

void ProductInPlaceEx( cMatrix< double >& m,
					  const cMatrix< double >& lm,
					  const cVector & rv,
					  bool use_mkl = false );

void ProductInPlaceEx( cMatrix< double >& m,
					  const cVector & lv,
					  const cMatrix< double >& rm,
					  bool use_mkl = false );

#endif
