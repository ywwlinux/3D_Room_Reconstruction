#include "ArcBall.h"

void CArcBall::mapToSphere(const Point& point, Vec3& vector)
{
	// Copy paramter into temp point
	Vec2 tempPoint(point.x, point.y);

	// Adjust point coords and scale down to range of [-1 ... 1]
	tempPoint.x() = (tempPoint.x() * _adjustWidth) - 1.0f;
	tempPoint.y() = 1.0f - (tempPoint.y() * _adjustHeight);

	// Compute the square of the length of the vector to the point from the
	// center
	double length = tempPoint.SquareMagnitude(); // (tempPoint.x * tempPoint.x) + (tempPoint.y * tempPoint.y);

	// If the point is mapped outside of the sphere... (length > radius
	// squared)
	if (length > 1.0f) {
		// Compute a normalizing factor (radius / sqrt(length))
		double norm = (double)(1.0 / sqrt(length));

		// Return the "normalized" vector, a point on the sphere
		vector.x() = tempPoint.x() * norm;
		vector.y() = tempPoint.y() * norm;
		vector.z() = 0.0f;
	}
	else // Else it's on the inside
	{
		// Return a vector to a point mapped inside the sphere sqrt(radius
		// squared - length)
		vector.x() = tempPoint.x();
		vector.y() = tempPoint.y();
		vector.z() = (float)sqrt(1.0f - length);
	}

	return;
}

void CArcBall::setBounds(double viewWidth, double viewHeight) 
{
	assert((viewWidth > 1.0f) && (viewHeight > 1.0f));

	// Set adjustment factor for width/height
	_adjustWidth = 1.0f / ((viewWidth - 1.0f) * 0.5f);
	_adjustHeight = 1.0f / ((viewHeight - 1.0f) * 0.5f);
}

void CArcBall::click(const Point& newPt) 
{
	mapToSphere(newPt, _stVec);
}

// Mouse drag, calculate rotation
void CArcBall::drag(const Point& newPt, Quat4f& newRot) 
{
	// Map the point to the sphere
	mapToSphere(newPt, _endVec);

	// Return the quaternion equivalent to the rotation
	Vec3 Perp;

	// Compute the vector perpendicular to the begin and end vectors
	Perp = Cross(_stVec, _endVec);

	// Compute the length of the perpendicular vector
	if (Perp.Magnitude() > FEPSILON) // if its non-zero
	{
		// We're ok, so return the perpendicular vector as the transform
		// after all
		newRot.x = Perp.x();
		newRot.y = Perp.y();
		newRot.z = Perp.z();
		// In the quaternion values, w is cosine (theta / 2), where
		// theta is rotation angle
		newRot.w = Dot(_stVec, _endVec); // Vector3f.dot(StVec, EnVec);
	}
	else // if its zero
	{
		// The begin and end vectors coincide, so return an identity
		// transform
		newRot.x = newRot.y = newRot.z = newRot.w = 0.0f;
	}
}

///////////////////////////////////////////////////////////////
// Implementation of Matrix4f  //

void Matrix4f::setIdentity()
{
	setZero();
	M00 = M11 = M22 = M33 = 1.0f;
}

void Matrix4f::setZero()
{
	M00 = M01 = M02 = M03 = M10 = M11 = M12 = M13 = M20 = M21 = M22 = M23 = M30 = M31 = M32 = M33 = 0.0f;
}

void Matrix4f::set(const Matrix4f& m1)
{
	M00 = m1.M00;
	M01 = m1.M01;
	M02 = m1.M02;
	M03 = m1.M03;
	M10 = m1.M10;
	M11 = m1.M11;
	M12 = m1.M12;
	M13 = m1.M13;
	M20 = m1.M20;
	M21 = m1.M21;
	M22 = m1.M22;
	M23 = m1.M23;
	M30 = m1.M30;
	M31 = m1.M31;
	M32 = m1.M32;
	M33 = m1.M33;
}

void Matrix4f::setRotation(const Quat4f& q1)
{
	double n, s;
	double xs, ys, zs;
	double wx, wy, wz;
	double xx, xy, xz;
	double yy, yz, zz;

	n = (q1.x * q1.x) + (q1.y * q1.y) + (q1.z * q1.z) + (q1.w * q1.w);
	s = (n > 0.0) ? (2.0 / n) : 0.0;

	xs = q1.x * s;
	ys = q1.y * s;
	zs = q1.z * s;
	wx = q1.w * xs;
	wy = q1.w * ys;
	wz = q1.w * zs;
	xx = q1.x * xs;
	xy = q1.x * ys;
	xz = q1.x * zs;
	yy = q1.y * ys;
	yz = q1.y * zs;
	zz = q1.z * zs;

	M00 = 1.0 - (yy + zz);
	M01 = xy - wz;
	M02 = xz + wy;
	M03 = .0;
	M10 = xy + wz;
	M11 = 1.0 - (xx + zz);
	M12 = yz - wx;
	M13 = .0;
	M20 = xz - wy;
	M21 = yz + wx;
	M22 = 1.0 - (xx + yy);
	M23 = .0;
	M30 = .0;
	M31 = .0;
	M32 = .0;
	M33 = 1.;
}

void Matrix4f::set(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13, double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33)
{
	this->M00 = m00;
	this->M01 = m01;
	this->M02 = m02;
	this->M03 = m03;
	this->M10 = m10;
	this->M11 = m11;
	this->M12 = m12;
	this->M13 = m13;
	this->M20 = m20;
	this->M21 = m21;
	this->M22 = m22;
	this->M23 = m23;
	this->M30 = m30;
	this->M31 = m31;
	this->M32 = m32;
	this->M33 = m33;
}

void Matrix4f::mul(Matrix4f m1, Matrix4f m2)
{
	// alias-safe way.
	set(m1.M00 * m2.M00 + m1.M01 * m2.M10 + m1.M02 * m2.M20 + m1.M03 * m2.M30, m1.M00 * m2.M01 + m1.M01 * m2.M11 + m1.M02 * m2.M21 + m1.M03 * m2.M31, m1.M00 * m2.M02 + m1.M01 * m2.M12 + m1.M02 * m2.M22 + m1.M03 * m2.M32, m1.M00 * m2.M03 + m1.M01 * m2.M13 + m1.M02 * m2.M23 + m1.M03 * m2.M33,

		m1.M10 * m2.M00 + m1.M11 * m2.M10 + m1.M12 * m2.M20 + m1.M13 * m2.M30, m1.M10 * m2.M01 + m1.M11 * m2.M11 + m1.M12 * m2.M21 + m1.M13 * m2.M31, m1.M10 * m2.M02 + m1.M11 * m2.M12 + m1.M12 * m2.M22 + m1.M13 * m2.M32, m1.M10 * m2.M03 + m1.M11 * m2.M13 + m1.M12 * m2.M23 + m1.M13 * m2.M33,

		m1.M20 * m2.M00 + m1.M21 * m2.M10 + m1.M22 * m2.M20 + m1.M23 * m2.M30, m1.M20 * m2.M01 + m1.M21 * m2.M11 + m1.M22 * m2.M21 + m1.M23 * m2.M31, m1.M20 * m2.M02 + m1.M21 * m2.M12 + m1.M22 * m2.M22 + m1.M23 * m2.M32, m1.M20 * m2.M03 + m1.M21 * m2.M13 + m1.M22 * m2.M23 + m1.M23 * m2.M33,

		m1.M30 * m2.M00 + m1.M31 * m2.M10 + m1.M32 * m2.M20 + m1.M33 * m2.M30, m1.M30 * m2.M01 + m1.M31 * m2.M11 + m1.M32 * m2.M21 + m1.M33 * m2.M31, m1.M30 * m2.M02 + m1.M31 * m2.M12 + m1.M32 * m2.M22 + m1.M33 * m2.M32, m1.M30 * m2.M03 + m1.M31 * m2.M13 + m1.M32 * m2.M23 + m1.M33 * m2.M33);
}

void Matrix4f::transpose()
{
	set(M00, M10, M20, M30, M01, M11, M21, M31, M02, M12, M22, M32, M03, M13, M23, M33);
}

void Matrix4f::set(double src[16])
{
	M00 = src[0];
	M10 = src[1];
	M20 = src[2];
	M30 = src[3];
	M01 = src[4];
	M11 = src[5];
	M21 = src[6];
	M31 = src[7];
	M02 = src[8];
	M12 = src[9];
	M22 = src[10];
	M32 = src[11];
	M03 = src[12];
	M13 = src[13];
	M23 = src[14];
	M33 = src[15];
}

void Matrix4f::get(double dest[16])
{
	dest[0] = M00;
	dest[1] = M10;
	dest[2] = M20;
	dest[3] = M30;
	dest[4] = M01;
	dest[5] = M11;
	dest[6] = M21;
	dest[7] = M31;
	dest[8] = M02;
	dest[9] = M12;
	dest[10] = M22;
	dest[11] = M32;
	dest[12] = M03;
	dest[13] = M13;
	dest[14] = M23;
	dest[15] = M33;
}

void Matrix4f::get(float dest[16])
{
	dest[0] = (float)M00;
	dest[1] = (float)M10;
	dest[2] = (float)M20;
	dest[3] = (float)M30;
	dest[4] = (float)M01;
	dest[5] = (float)M11;
	dest[6] = (float)M21;
	dest[7] = (float)M31;
	dest[8] = (float)M02;
	dest[9] = (float)M12;
	dest[10] = (float)M22;
	dest[11] = (float)M32;
	dest[12] = (float)M03;
	dest[13] = (float)M13;
	dest[14] = (float)M23;
	dest[15] = (float)M33;
}