#include <cMatrix.h>

void ProductInPlaceEx( cMatrix< double >& m,
					  const cMatrix< double >& lm,
					  const cMatrix< double> & rm,
					  bool use_mkl )
{
	ProductInPlace( m, lm, rm );
}


void ProductInPlace( cMatrix< double >& m, const cVector& l, const cMatrix< double >& r)
{
	size_t l_size = (size_t)l.getSize();
	size_t r_cols = r.ncols();
	size_t r_rows = r.nrows();

	yr_assert( l_size == r_rows );

	m.resize( 1, r_cols );

	for( size_t j = 0; j < r_cols; ++j )
	{
		for( size_t k = 0; k < l_size; ++k )
			m(0,j) += l[k] * r(k,j);
	}
}

void ProductInPlace( cMatrix< double >& m, const cMatrix< double >& l, const cVector& r )
{
	size_t l_cols = l.ncols();
	size_t l_rows = l.nrows();
	size_t r_size = (size_t)r.getSize();

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

void ProductInPlaceEx( cMatrix< double >& m,
					  const cMatrix< double >& lm,
					  const cVector& rv,
					  bool use_mkl )
{
	ProductInPlace( m, lm, rv );
}

void ProductInPlaceEx( cMatrix< double >& m, 
					  const cVector& lv,
					  const cMatrix< double >& rm,
					  bool use_mkl )
{
	ProductInPlace( m, lv, rm );
}