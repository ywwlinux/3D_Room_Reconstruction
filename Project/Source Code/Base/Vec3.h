/*******************************************************************
* Author	: wwyang
* Date		: 2006.3.21
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : Structure for 3D vector 
* Version   : 1.0
*********************************************************************/

#ifndef _VEC3_H_
#define _VEC3_H_

#include "Base.h"	// For pre-definition
#include "math.h"	// For math functions
#include "assert.h"

namespace base{

class Vec3{
public:
	M_FLOAT _x, _y, _z;

public:
	// constructor
	Vec3();
	Vec3(M_FLOAT x, M_FLOAT y, M_FLOAT z);
	Vec3(M_FLOAT xyz[]);
	Vec3(const Vec3& vec);

	// assignment
	inline Vec3& operator = (const Vec3& vec);

	// assign value
	inline void Set(M_FLOAT x, M_FLOAT y, M_FLOAT z);

	// get&set
	inline const M_FLOAT& x() const;
	inline M_FLOAT& x();
	inline const M_FLOAT& y() const;
	inline M_FLOAT& y();
	inline const M_FLOAT& z() const;
	inline M_FLOAT& z();

	// operations
	inline const M_FLOAT& operator [](int index) const;
	inline M_FLOAT& operator[](int index);

	inline friend Vec3 operator + (const Vec3& vec1, const Vec3& vec2);
	inline friend Vec3 operator - (const Vec3& vec1, const Vec3& vec2);
	inline friend Vec3 operator * (const Vec3& vec, M_FLOAT f);
	inline friend Vec3 operator * (M_FLOAT f, const Vec3& vec);
	inline friend Vec3 operator / (const Vec3& vec, M_FLOAT f);

	inline friend Vec3 operator - (const Vec3& a);

	inline Vec3& operator += (const Vec3& vec);
	inline Vec3& operator -= (const Vec3& vec);
	inline Vec3& operator *= (M_FLOAT f);
	inline Vec3& operator /= (M_FLOAT f);

	// utilities
	inline M_FLOAT Magnitude() const;
	inline M_FLOAT SquareMagnitude() const;
	inline Vec3 Normalize() const;

	// vector operations
	inline friend M_FLOAT Dot(const Vec3& vec, const Vec3& vec2);
	inline friend Vec3 Cross(const Vec3& vec, const Vec3& vec2);

};

inline Vec3::Vec3() : _x(0), _y(0), _z(0)
{

}

inline Vec3::Vec3(M_FLOAT x, M_FLOAT y, M_FLOAT z) : _x(x), _y(y), _z(z) 
{

}

inline Vec3::Vec3(M_FLOAT xyz[]) : _x(xyz[0]), _y(xyz[1]), _z(xyz[2])
{

}

inline Vec3::Vec3(const Vec3& vec)
{
	operator = (vec);
}

inline Vec3& Vec3::operator =(const Vec3& vec)
{
	_x = vec._x;
	_y = vec._y;
	_z = vec._z;

	return *this;
}

inline void Vec3::Set(M_FLOAT x, M_FLOAT y, M_FLOAT z)
{
	_x = x;
	_y = y;
	_z = z;
}

inline const M_FLOAT& Vec3::x() const
{
	return _x;
}

inline M_FLOAT& Vec3::x()
{
	return _x;
}

inline const M_FLOAT& Vec3::y() const
{
	return _y;
}

inline M_FLOAT& Vec3::y()
{
	return _y;
}

inline const M_FLOAT& Vec3::z() const
{
	return _z;
}

inline M_FLOAT& Vec3::z()
{
	return _z;
}

inline const M_FLOAT& Vec3::operator [](int index) const
{
	if(index == 0){
		return _x;
	}
	else if(index == 1){
		return _y;
	}
	else{
		assert(index == 2);
		return _z;
	}
}

inline M_FLOAT& Vec3::operator[](int index)
{
	if(index == 0){
		return _x;
	}
	else if(index == 1){
		return _y;
	}
	else{
		assert(index == 2);
		return _z;
	}
}

inline Vec3 operator + (const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1._x + vec2._x, vec1._y + vec2._y, vec1._z + vec2._z);
}

inline Vec3 operator - (const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1._x - vec2._x, vec1._y - vec2._y, vec1._z - vec2._z);
}

inline Vec3 operator * (const Vec3& vec, M_FLOAT f)
{
	return Vec3(vec._x * f, vec._y * f, vec._z * f);
}

inline Vec3 operator * (M_FLOAT f, const Vec3& vec)
{
	return operator *(vec, f);
}

inline Vec3 operator / (const Vec3& vec, M_FLOAT f)
{
#ifdef _DEBUG
	assert(f != 0);
#endif

	return operator *(vec, 1./f);
}

inline Vec3 operator - (const Vec3& a)
{
	return Vec3(-a._x, -a._y, -a._z);
}

inline Vec3& Vec3::operator += (const Vec3& vec)
{
	_x += vec._x;
	_y += vec._y;
	_z += vec._z;
	return *this;
}

inline Vec3& Vec3::operator -= (const Vec3& vec)
{
	_x -= vec._x;
	_y -= vec._y;
	_z -= vec._z;
	return *this;
}

inline Vec3& Vec3::operator *= (M_FLOAT f)
{
	_x *= f;
	_y *= f;
	_z *= f;
	return *this;
}

inline Vec3& Vec3::operator /= (M_FLOAT f)
{
#ifdef _DEBUG
	assert(f != 0);
#endif

	return operator *= (1./f);
}

inline M_FLOAT Vec3::Magnitude() const
{
	M_FLOAT fMul = _x * _x + _y * _y + _z * _z;
	return (M_FLOAT)sqrt(double(fMul));
}

inline M_FLOAT Vec3::SquareMagnitude() const
{
	return _x * _x + _y * _y + _z * _z;
}

inline Vec3 Vec3::Normalize() const
{
	M_FLOAT fMag = Magnitude();
	Vec3 normVec;
	if(fMag == 0){ 
		normVec._x = 0;
		normVec._y = 0;
		normVec._z = 0;
	}
	else{
		normVec._x = _x / fMag;
		normVec._y = _y / fMag;
		normVec._z = _z / fMag;
	}

	return normVec;
}

inline M_FLOAT Dot(const Vec3& vec1, const Vec3& vec2)
{
	return vec1._x * vec2._x + vec1._y * vec2._y + vec1._z * vec2._z;
}

inline Vec3 Cross(const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1._y * vec2._z - vec1._z * vec2._y, vec1._z * vec2._x - vec1._x * vec2._z, vec1._x * vec2._y - vec1._y * vec2._x);
}

} // namespace base
#endif
