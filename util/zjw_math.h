/*!
 * \file zjw_math.h
 *
 * \author zhoujiawei
 * \date 三月 2017
 * modify frmm other open source code.
 * 
 */
#pragma once

//#define _USE_MATH_DEFINES
#include <math.h>
const double Epsilon = 1e-5;
class  Vec3;
typedef Vec3 Color3;
typedef Vec3 Point3;
static const double PI = acos(-1.0);

inline bool DoubleEquals(double a, double b, double eps = Epsilon)
{
	if (fabs(a - b) < eps) {
		return true;
	}
	return false;
}
inline int DoubleCompare(double a, double b, double eps = Epsilon)
{
	if (DoubleEquals(a, b, eps)) {
		return 0;
	}
	if (a < b) {
		return -1;
	}
	return 1;
}
inline int Round(double x)
{
	return (int)(x+0.5);
}

class Vec2
{
public:
	double x, y;
	/*union {
		struct { double x, y; };
		struct { double s, t; };
		struct { double r, g; };
		double num[2];
	};*/
	Vec2() : x(0.0f), y(0.0f) { }
	~Vec2() { }
	Vec2(double num) : x(num), y(num) { }
	Vec2(double x, double y) : x(x), y(y) { }
	Vec2(const Vec2 &u) : x(u.x), y(u.y) { }
	inline Vec2& operator = (const Vec2 &u) { x = u.x; y = u.y; return *this; }
	inline Vec2 operator - () { return Vec2(-x, -y); }
	inline Vec2& operator += (double num) { x += num; y += num; return *this; }
	inline Vec2& operator += (const Vec2 &u) { x += u.x; y += u.y; return *this; }
	inline Vec2& operator -= (double num) { x -= num; y -= num; return *this; }
	inline Vec2& operator -= (const Vec2 &u) { x -= u.x; y -= u.y; return *this; }
	inline Vec2& operator *= (double num) { x *= num; y *= num; return *this; }
	inline Vec2& operator *= (const Vec2 &u) { x *= u.x; y *= u.y; return *this; }
	inline Vec2& operator /= (double num) { x /= num; y /= num; return *this; }
	inline Vec2& operator /= (const Vec2 &u) { x /= u.x; y /= u.y; return *this; }
	inline bool operator == (const Vec2 &u) { return DoubleEquals(x, u.x) && DoubleEquals(y, u.y); }
	inline bool operator != (const Vec2 &u) { return !(DoubleEquals(x, u.x) && DoubleEquals(y, u.y)); }
	friend inline Vec2 operator + (const Vec2 &u, double num) { return Vec2(u.x + num, u.y + num); }
	friend inline Vec2 operator + (double num, const Vec2 &u) { return Vec2(num + u.x, num + u.y); }
	friend inline Vec2 operator + (const Vec2 &u, const Vec2 &v) { return Vec2(u.x + v.x, u.y + v.y); }
	friend inline Vec2 operator - (const Vec2 &u, double num) { return Vec2(u.x - num, u.y - num); }
	friend inline Vec2 operator - (double num, const Vec2 &u) { return Vec2(num - u.x, num - u.y); }
	friend inline Vec2 operator - (const Vec2 &u, const Vec2 &v) { return Vec2(u.x - v.x, u.y - v.y); }
	friend inline Vec2 operator * (const Vec2 &u, double num) { return Vec2(u.x * num, u.y * num); }
	friend inline Vec2 operator * (double num, const Vec2 &u) { return Vec2(num * u.x, num * u.y); }
	friend inline Vec2 operator * (const Vec2 &u, const Vec2 &v) { return Vec2(u.x * v.x, u.y * v.y); }
	friend inline Vec2 operator / (const Vec2 &u, double num) { return Vec2(u.x / num, u.y / num); }
	friend inline Vec2 operator / (double num, const Vec2 &u) { return Vec2(num / u.x, num / u.y); }
	friend inline Vec2 operator / (const Vec2 &u, const Vec2 &v) { return Vec2(u.x / v.x, u.y / v.y); }
};

// ----------------------------------------------------------------------------------------------------------------------------

class Vec3
{
public:
	double x, y, z;

	/*union {
		struct { double x, y, z; };
		struct { double s, t, p; };
		struct { double r, g, b; };
		double num[3];
	};*/
	static const Vec3 NONE,BLACK,WHITE;

	Vec3() : x(0.0f), y(0.0f), z(0.0f) { }
	~Vec3() { }
	Vec3(double num) : x(num), y(num), z(num) { }
	Vec3(double x, double y, double z) : x(x), y(y), z(z) { }
	Vec3(const Vec2 &u, double z) : x(u.x), y(u.y), z(z) { }
	Vec3(const Vec3 &u) : x(u.x), y(u.y), z(u.z) { }
	
	inline Vec3& operator = (const Vec3 &u) { x = u.x; y = u.y; z = u.z; return *this; }
	inline Vec3 operator - () { return Vec3(-x, -y, -z); }
	inline double* operator & () { return (double*)this; }
	inline operator Vec2 () { return *(Vec2*)this; }
	inline Vec3& operator += (double num) { x += num; y += num; z += num; return *this; }
	inline Vec3& operator += (const Vec3 &u) { x += u.x; y += u.y; z += u.z; return *this; }
	inline Vec3& operator -= (double num) { x -= num; y -= num; z -= num; return *this; }
	inline Vec3& operator -= (const Vec3 &u) { x -= u.x; y -= u.y; z -= u.z; return *this; }
	inline Vec3& operator *= (double num) { x *= num; y *= num; z *= num; return *this; }
	inline Vec3& operator *= (const Vec3 &u) { x *= u.x; y *= u.y; z *= u.z; return *this; }
	inline Vec3& operator /= (double num) { x /= num; y /= num; z /= num; return *this; }
	inline Vec3& operator /= (const Vec3 &u) { x /= u.x; y /= u.y; z /= u.z; return *this; }
	inline bool operator == (const Vec3 &u) { return DoubleEquals(x, u.x) && DoubleEquals(y, u.y) && DoubleEquals(z, u.z); }
	inline bool operator != (const Vec3 &u) { return !(DoubleEquals(x, u.x) && DoubleEquals(y, u.y) && DoubleEquals(z, u.z)); }
	friend inline Vec3 operator + (const Vec3 &u, double num) { return Vec3(u.x + num, u.y + num, u.z + num); }
	friend inline Vec3 operator + (double num, const Vec3 &u) { return Vec3(num + u.x, num + u.y, num + u.z); }
	friend inline Vec3 operator + (const Vec3 &u, const Vec3 &v) { return Vec3(u.x + v.x, u.y + v.y, u.z + v.z); }
	friend inline Vec3 operator - (const Vec3 &u, double num) { return Vec3(u.x - num, u.y - num, u.z - num); }
	friend inline Vec3 operator - (double num, const Vec3 &u) { return Vec3(num - u.x, num - u.y, num - u.z); }
	friend inline Vec3 operator - (const Vec3 &u, const Vec3 &v) { return Vec3(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline Vec3 operator * (const Vec3 &u, double num) { return Vec3(u.x * num, u.y * num, u.z * num); }
	friend inline Vec3 operator * (double num, const Vec3 &u) { return Vec3(num * u.x, num * u.y, num * u.z); }
	friend inline Vec3 operator * (const Vec3 &u, const Vec3 &v) { return Vec3(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline Vec3 operator / (const Vec3 &u, double num) { return Vec3(u.x / num, u.y / num, u.z / num); }
	friend inline Vec3 operator / (double num, const Vec3 &u) { return Vec3(num / u.x, num / u.y, num / u.z); }
	friend inline Vec3 operator / (const Vec3 &u, const Vec3 &v) { return Vec3(u.x / v.x, u.y / v.y, u.z / v.z); }

	//----------针对应用扩展，可以删除-----------------------
	Vec3(const float p2[3]) : x(p2[0]), y(p2[1]), z(p2[2]) {}
	operator double *() { return &x; }
	operator const double*() const { return &x; }
	bool operator< (const Vec3& p2) const { return x < p2.x && y < p2.y && z < p2.z; }
	bool operator>=(const Vec3& p2) const { return x >= p2.x && y >= p2.y && z >= p2.z; }
	//--------------------end add----------------------------------

	Vec3 multiple(const Vec3 &u) { return Vec3(x*u.x, y*u.y, z*u.z); };
	Vec3 flip() { return Vec3(-x, -y, -z); };

	//归一化1
	inline void normalize()
	{
		(*this) = (*this) * (1.0f / sqrt(x * x + y * y + z * z));
	}

	//两点之间的点乘
	inline double Dot(const Vec3 &v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	
	//两点间的距离
	inline double Distance(const Vec3 &u)
	{
		return sqrt(x * u.x + y * u.y + z * u.z);
	}
};


// ----------------------------------------------------------------------------------------------------------------------------

class Vec4
{
public:
	double x, y, z, w;

	/*union {
		struct { double x, y, z, w; };
		struct { double s, t, p, q; };
		struct { double r, g, b, a; };
		double num[4];
	};*/
	Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	~Vec4() { }
	Vec4(double num) : x(num), y(num), z(num), w(num) { }
	Vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) { }
	Vec4(const Vec2 &u, double z, double w) : x(u.x), y(u.y), z(z), w(w) { }
	Vec4(const Vec3 &u, double w) : x(u.x), y(u.y), z(u.z), w(w) { }
	Vec4(const Vec4 &u) : x(u.x), y(u.y), z(u.z), w(u.w) { }
	inline Vec4& operator = (const Vec4 &u) { x = u.x; y = u.y; z = u.z; w = u.w; return *this; }
	inline Vec4 operator - () { return Vec4(-x, -y, -z, -w); }
	inline double* operator & () { return (double*)this; }
	inline operator Vec2 () { return *(Vec2*)this; }
	inline operator Vec3 () { return *(Vec3*)this; }
	inline Vec4& operator += (double num) { x += num; y += num; z += num; w += num; return *this; }
	inline Vec4& operator += (const Vec4 &u) { x += u.x; y += u.y; z += u.z; w += u.w; return *this; }
	inline Vec4& operator -= (double num) { x -= num; y -= num; z -= num; w -= num; return *this; }
	inline Vec4& operator -= (const Vec4 &u) { x -= u.x; y -= u.y; z -= u.z; w -= u.w; return *this; }
	inline Vec4& operator *= (double num) { x *= num; y *= num; z *= num; w *= num; return *this; }
	inline Vec4& operator *= (const Vec4 &u) { x *= u.x; y *= u.y; z *= u.z; w *= u.w; return *this; }
	inline Vec4& operator /= (double num) { x /= num; y /= num; z /= num; w /= num; return *this; }
	inline Vec4& operator /= (const Vec4 &u) { x /= u.x; y /= u.y; z /= u.z; w /= u.w; return *this; }
	inline bool operator == (const Vec4 &u) { return DoubleEquals(x, u.x) && DoubleEquals(y, u.y) && DoubleEquals(z, u.z) && DoubleEquals(w, u.w); }
	inline bool operator != (const Vec4 &u) { return !(DoubleEquals(x, u.x) && DoubleEquals(y, u.y) && DoubleEquals(z, u.z) && DoubleEquals(w, u.w)); }
	friend inline Vec4 operator + (const Vec4 &u, double num) { return Vec4(u.x + num, u.y + num, u.z + num, u.w + num); }
	friend inline Vec4 operator + (double num, const Vec4 &u) { return Vec4(num + u.x, num + u.y, num + u.z, num + u.w); }
	friend inline Vec4 operator + (const Vec4 &u, const Vec4 &v) { return Vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w); }
	friend inline Vec4 operator - (const Vec4 &u, double num) { return Vec4(u.x - num, u.y - num, u.z - num, u.w - num); }
	friend inline Vec4 operator - (double num, const Vec4 &u) { return Vec4(num - u.x, num - u.y, num - u.z, num - u.w); }
	friend inline Vec4 operator - (const Vec4 &u, const Vec4 &v) { return Vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w); }
	friend inline Vec4 operator * (const Vec4 &u, double num) { return Vec4(u.x * num, u.y * num, u.z * num, u.w * num); }
	friend inline Vec4 operator * (double num, const Vec4 &u) { return Vec4(num * u.x, num * u.y, num * u.z, num * u.w); }
	friend inline Vec4 operator * (const Vec4 &u, const Vec4 &v) { return Vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w); }
	friend inline Vec4 operator / (const Vec4 &u, double num) { return Vec4(u.x / num, u.y / num, u.z / num, u.w / num); }
	friend inline Vec4 operator / (double num, const Vec4 &u) { return Vec4(num / u.x, num / u.y, num / u.z, num / u.w); }
	friend inline Vec4 operator / (const Vec4 &u, const Vec4 &v) { return Vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w); }
};

inline Vec2 Normalize(const Vec2 &u)
{
	return u * (1.0f / sqrt(u.x * u.x + u.y * u.y));
}
inline Vec3 Normalize(const Vec3 &u)
{
	return u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
}
inline double Dot(const Vec2 &u, const Vec2 &v)
{
	return u.x * v.x + u.y * v.y;
}
inline double Length(const Vec2 &u)
{
	return sqrt(u.x * u.x + u.y * u.y);
}
inline double Length2(const Vec2 &u)
{
	return u.x * u.x + u.y * u.y;
}
//叉乘
inline Vec3 Cross(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}
inline double Dot(const Vec3 &u, const Vec3 &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}
inline double Length(const Vec3 &u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}
inline double Length2(const Vec3 &u)
{
	return u.x * u.x + u.y * u.y + u.z * u.z;
}

inline Vec3 Limit(const Vec3& src, double bottom, double top)
{
	Vec3 result;
	if (src.x < bottom) {
		result.x = bottom;
	} else if (src.x > top) {
		result.x = top;
	} else {
		result.x = src.x;
	}
	if (src.y < bottom) {
		result.y = bottom;
	} else if (src.y > top) {
		result.y = top;
	} else {
		result.y = src.y;
	}
	if (src.z < bottom) {
		result.z = bottom;
	} else if (src.z > top) {
		result.z = top;
	} else {
		result.z = src.z;
	}
	return result;
}