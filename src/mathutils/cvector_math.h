#ifndef __CVECTOR_MATH__
#define __CVECTOR_MATH__

#define FUZZ 1e-8
#define YR_SQRTPI    1.772453850905516
#define YR_SQRT2PI    2.50662827463100050242E0

double yr_log( double x, double limit = FUZZ );
double normal( double );
double normal_inv( double );
double normal_inv2( double );

#endif
