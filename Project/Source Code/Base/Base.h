/*******************************************************************
* Author	: wwyang
* Date		: 2006.3.21
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : Something pre-defined
* Version   : 1.0
*********************************************************************/

#ifndef _BASE_H_

#pragma warning(disable: 4244 4267 4018 4800 4305)

typedef double M_FLOAT;

namespace base{};
using namespace base;

#define FEPSILON 1.0e-6
#define FABS(a) ( (a) >= 0 ? (a) : -(a) )
#define FLOAT_EQUAL(a, b) ( ( (a)>(b) ? ((a)-(b)) : ((b)-(a)) ) <= FEPSILON ? true : false ) 
#define PI 3.1415926536

#endif