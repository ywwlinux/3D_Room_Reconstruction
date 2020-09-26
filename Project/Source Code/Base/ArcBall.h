#ifndef _ARCBALL_H_
#define _ARCBALL_H_

#include "Vec2.h"
#include "Vec3.h"
#include "math.h"

struct Point {
	int x, y;
};

class Quat4f {
public:
	double x, y, z, w;

	Quat4f(): x(0), y(0), z(0), w(0) {	}

	Quat4f(double angle, float x, float y, float z) 
	{
		double s = sqrt(x * x + y * y + z * z);
		double c = 1.0 / s;

		x *= c;
		y *= c;
		z *= c;

		double omega = (double)(-0.5f * angle);
		s = (double)sin(omega);

		x = (float)(s * x);
		y = (float)(s * y);
		z = (float)(s * z);
		w = (float)cos(omega);
	}
};

class CArcBall {
public:
	CArcBall(double viewWidth, double viewHeight)
	{
		setBounds(viewWidth, viewHeight);
	}
	CArcBall()
	{
		setBounds(800, 600);
	}
	~CArcBall() {}

	void setBounds(double viewWidth, double viewHeight);
	void click(const Point& newPt);
	void drag(const Point& NewPt, Quat4f &NewRot);

protected:
	void mapToSphere(const Point& point, Vec3& vector);

private:
	Vec3 _stVec; // Saved click vector
	Vec3 _endVec; // Saved drag vector
	double _adjustWidth; // Mouse bounds width
	double _adjustHeight; // Mouse bounds height
};

class Matrix4f {
public:
	Matrix4f() 
	{
		setIdentity();
	}

	void setIdentity();
	void setZero();
	void setRotation(const Quat4f& q1);
	void set(const Matrix4f& m1);
	void set(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13, double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33);
	void mul(Matrix4f m1, Matrix4f m2);
	void transpose();

	// Get the matrix elements -- the order is in column-major 
	void get(double dest[16]);
	void get(float dest[16]);
	void set(double src[16]);

private:
	double M00;
	double M10;
	double M20;
	double M30;
	double M01;
	double M11;
	double M21;
	double M31;
	double M02;
	double M12;
	double M22;
	double M32;
	double M03;
	double M13;
	double M23;
	double M33;
};

#endif // !_ARCBALL_H_

