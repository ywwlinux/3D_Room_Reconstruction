/*******************************************************************
* Author	: wwyang
* Date		: 2006.3.21
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : Structure for 2D vector 
* Version   : 1.0
*********************************************************************/

#ifndef _VEC2_H_
#define _VEC2_H_

#include "Base.h"	// For pre-definition
#include "math.h"   // For math functions
#include "assert.h"

namespace base{

class Vec2{
public:
	M_FLOAT _x, _y;

public:
	// constructor
	Vec2();
	Vec2(M_FLOAT x, M_FLOAT y);
	Vec2(M_FLOAT xy[]);
	Vec2(const Vec2& vec);

	// assignment
	Vec2& operator = (const Vec2& vec);

	// assign value
	void Set(M_FLOAT x, M_FLOAT y);

	// get&set
	const M_FLOAT& x() const;
	M_FLOAT& x();
	const M_FLOAT& y() const;
	M_FLOAT& y();

	// operations
	const M_FLOAT& operator [](int index) const;
	M_FLOAT& operator[](int index);

	friend Vec2 operator + (const Vec2& vec1, const Vec2& vec2);
	friend Vec2 operator - (const Vec2& vec1, const Vec2& vec2);
	friend Vec2 operator * (const Vec2& vec, M_FLOAT f);
	friend Vec2 operator * (M_FLOAT f, const Vec2& vec);
	friend Vec2 operator / (const Vec2& vec, M_FLOAT f);

	friend Vec2 operator - (const Vec2& a);

	Vec2& operator += (const Vec2& vec);
	Vec2& operator -= (const Vec2& vec);
	Vec2& operator *= (M_FLOAT f);
	Vec2& operator /= (M_FLOAT f);

	// utilities
	M_FLOAT Magnitude() const;
	M_FLOAT SquareMagnitude() const;
	Vec2 Normalize() const;

	// vector operations
	friend M_FLOAT Dot(const Vec2& vec1, const Vec2& vec2);
	friend M_FLOAT Cross(const Vec2& vec1, const Vec2& vec2);

};

inline Vec2::Vec2() : _x(0), _y(0)
{

}

inline Vec2::Vec2(M_FLOAT x, M_FLOAT y) : _x(x), _y(y)
{

}

inline Vec2::Vec2(M_FLOAT xy[]) : _x(xy[0]), _y(xy[1])
{

}

inline Vec2::Vec2(const Vec2& vec)
{
	operator = (vec);
}

inline Vec2& Vec2::operator =(const Vec2& vec)
{
	_x = vec._x;
	_y = vec._y;

	return *this;
}

inline void Vec2::Set(M_FLOAT x, M_FLOAT y)
{
	_x = x;
	_y = y;
}

inline const M_FLOAT& Vec2::x() const
{
	return _x;
}

inline M_FLOAT& Vec2::x()
{
	return _x;
}

inline const M_FLOAT& Vec2::y() const
{
	return _y;
}

inline M_FLOAT& Vec2::y()
{
	return _y;
}

inline const M_FLOAT& Vec2::operator [](int index) const
{
	if(index == 0){
		return _x;
	}
	else{
		assert(index == 1);
		return _y;
	}
}

inline M_FLOAT& Vec2::operator[](int index)
{
	if(index == 0){
		return _x;
	}
	else{
		assert(index == 1);
		return _y;
	}
}

inline Vec2 operator + (const Vec2& vec1, const Vec2& vec2)
{
	return Vec2(vec1._x + vec2._x, vec1._y + vec2._y);
}

inline Vec2 operator - (const Vec2& vec1, const Vec2& vec2)
{
	return Vec2(vec1._x - vec2._x, vec1._y - vec2._y);
}

inline Vec2 operator * (const Vec2& vec, M_FLOAT f)
{
	return Vec2(vec._x * f, vec._y * f);
}

inline Vec2 operator * (M_FLOAT f, const Vec2& vec)
{
	return operator *(vec, f);
}

inline Vec2 operator / (const Vec2& vec, M_FLOAT f)
{
#ifdef _DEBUG
	assert(f != 0);
#endif

	return operator *(vec, 1./f);
}

inline Vec2 operator - (const Vec2& a)
{
	return Vec2(-a._x, -a._y);
}

inline Vec2& Vec2::operator += (const Vec2& vec)
{
	_x += vec._x;
	_y += vec._y;
	return *this;
}

inline Vec2& Vec2::operator -= (const Vec2& vec)
{
	_x -= vec._x;
	_y -= vec._y;
	return *this;
}

inline Vec2& Vec2::operator *= (M_FLOAT f)
{
	_x *= f;
	_y *= f;
	return *this;
}

inline Vec2& Vec2::operator /= (M_FLOAT f)
{
#ifdef _DEBUG
	assert(f != 0);
#endif

	return operator *= (1./f);
}

inline M_FLOAT Vec2::Magnitude() const
{
	M_FLOAT fMul = _x * _x + _y * _y;
	return (M_FLOAT)sqrt(double(fMul));
}

inline M_FLOAT Vec2::SquareMagnitude() const
{
	return _x * _x + _y * _y;
}

inline Vec2 Vec2::Normalize() const
{
	M_FLOAT fMag = Magnitude();
	Vec2 normVec;
	if(fMag == 0){ 
		normVec._x = 0;
		normVec._y = 0;
	}
	else{
		normVec._x = _x / fMag;
		normVec._y = _y / fMag;
	}

	return normVec;
}

inline M_FLOAT Dot(const Vec2& vec1, const Vec2& vec2)
{
	return vec1._x * vec2._x + vec1._y * vec2._y;
}

inline M_FLOAT Cross(const Vec2& vec1, const Vec2& vec2)
{
	return vec1._x * vec2._y - vec1._y * vec2._x;
}

} // namespace base

#endif

//////////////////////////////////////////////////////////////////////////
// tips:
// 1. Note: just the instance var can be (const) referencedly return 