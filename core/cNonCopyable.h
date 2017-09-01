#ifndef __CNONCOPYABLE_H__
#define __CNONCOPYABLE_H__


class cNonCopyable
{
public:
	cNonCopyable() {}
	virtual ~cNonCopyable() {}

private:
	cNonCopyable( const cNonCopyable& );
	const cNonCopyable& operator = ( const cNonCopyable& );
};

#endif