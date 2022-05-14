#include "Vector.h"

vec2::vec2()
	: x(0), y(0)
{
}

vec2::vec2(float f)
	: x(f), y(f)
{
}

vec2::vec2(float x, float y)
	: x(x), y(y)
{
}

vec2::vec2(const vec2& vector)
	: x(vector.x), y(vector.y)
{
}

vec2 vec2::lerp(const vec2& start, const vec2& end, float delta)
{
	return start * (1 - delta) + end * delta;
}

float vec2::dot(const vec2& v1, const vec2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float vec2::angle(const vec2& v1, const vec2& v2)
{
	return acosf(dot(v1, v2) / (v1.mag() * v2.mag()));
}

vec2 vec2::operator+(const vec2& v) const
{
	return vec2(x+v.x, y+v.y);
}

vec2 vec2::operator-(const vec2& v) const
{
	return vec2(x - v.x, y - v.y);
}

vec2 vec2::operator*(const vec2& v) const
{
	return vec2(x * v.x, y * v.y);
}

vec2 vec2::operator/(const vec2& v) const
{
	return vec2(x / v.x, y / v.y);
}

vec2& vec2::operator+=(const vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

vec2& vec2::operator-=(const vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

vec2& vec2::operator*=(const vec2& v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

vec2& vec2::operator/=(const vec2& v)
{
	x /= v.x;
	y /= v.y;
	return *this;
}

vec2& vec2::operator=(const vec2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

bool vec2::operator==(const vec2& v) const
{
	return x==v.x && y==v.y;
}

float vec2::mag() const
{
	return sqrtf(x * x + y * y);
}

vec2 vec2::normal() const
{
	return *this / mag();
}

vec2& vec2::normalize()
{
	return *this /= mag();
}

vec3::vec3()
	: x(0), y(0), z(0)
{
}

vec3::vec3(float f)
	: x(f), y(f), z(f)
{
}

vec3::vec3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

vec3::vec3(const vec2& xy, float z)
	: x(xy.x), y(xy.y), z(z)
{
}

vec3::vec3(float x, const vec2& yz)
	: x(x), y(yz.x), z(yz.y)
{
}

vec3::vec3(const vec3& vector)
	: x(vector.x), y(vector.y), z(vector.z)
{
}

vec3 vec3::lerp(const vec3& start, const vec3& end, float delta)
{
	return start * (1 - delta) + end * delta;
}

float vec3::dot(const vec3& v1, const vec3& v2)
{
	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

vec3 vec3::cross(const vec3& v1, const vec3& v2)
{
	return vec3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

float vec3::angle(const vec3& v1, const vec3& v2)
{
	return acosf(dot(v1, v2) / (v1.mag() * v2.mag()));
}

vec3 vec3::operator+(const vec3& v) const
{
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3& v) const
{
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(const vec3& v) const
{
	return vec3(x * v.x, y * v.y, z * v.z);
}

vec3 vec3::operator/(const vec3& v) const
{
	return vec3(x / v.x, y / v.y, z / v.z);
}

vec3& vec3::operator+=(const vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vec3& vec3::operator-=(const vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3& vec3::operator*=(const vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

vec3& vec3::operator/=(const vec3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

vec3& vec3::operator=(const vec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

bool vec3::operator==(const vec3& v) const
{
	return x==v.x && y==v.y && z==v.z;
}

float vec3::mag() const
{
	return sqrtf(x*x+y*y+z*z);
}

vec3 vec3::normal() const
{
	return *this / mag();
}

vec3& vec3::normalize()
{
	return *this /= mag();
}

vec2 vec3::xy()
{
	return vec2(x, y);
}

vec2 vec3::yz()
{
	return vec2(y, z);
}

vec2 vec3::xz()
{
	return vec2(x, z);
}

vec2 vec3::yx()
{
	return vec2(y, x);
}

vec2 vec3::zy()
{
	return vec2(z, y);
}

vec2 vec3::zx()
{
	return vec2(z, x);
}


vec4::vec4()
	: x(0), y(0), z(0), w(0)
{
}

vec4::vec4(float f)
	: x(f), y(f), z(f), w(f)
{
}

vec4::vec4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{
}

vec4::vec4(const vec2& xy, const vec2& zw)
	: x(xy.x), y(xy.y), z(zw.x), w(zw.y)
{
}

vec4::vec4(const vec2& xy, float z, float w)
	: x(xy.x), y(xy.y), z(z), w(w)
{
}

vec4::vec4(float x, float y, const vec2& zw)
	: x(x), y(y), z(zw.x), w(zw.y)
{
}

vec4::vec4(const vec3& xyz, float w)
	: x(xyz.x), y(xyz.y), z(xyz.z), w(w)
{
}

vec4::vec4(float x, const vec3& yzw)
	: x(x), y(yzw.x), z(yzw.y), w(yzw.z)
{
}

vec4::vec4(const vec4&& vector) noexcept
	: x(vector.x), y(vector.y), z(vector.z), w(vector.w)
{
}

vec4 vec4::lerp(const vec4& start, const vec4& end, float delta)
{
	return start * (1 - delta) + end * delta;
}

float vec4::dot(const vec4& v1, const vec4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

vec4 vec4::cross(const vec4& v1, const vec4& v2, const vec4& v3)
{
	return vec4(
		v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y),
		-(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z)),
		v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y),
		-(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y))
	);
}

float vec4::angle(const vec4& v1, const vec4& v2)
{
	return acosf(dot(v1, v2) / (v1.mag() * v2.mag()));
}

vec4 vec4::operator+(const vec4& v) const
{
	return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

vec4 vec4::operator-(const vec4& v) const
{
	return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

vec4 vec4::operator*(const vec4& v) const
{
	return vec4(x * v.x, y * v.y, z * v.z, w * v.w);
}

vec4 vec4::operator/(const vec4& v) const
{
	return vec4(x / v.x, y / v.y, z / v.z, w / v.w);
}

vec4& vec4::operator+=(const vec4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

vec4& vec4::operator-=(const vec4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

vec4& vec4::operator*=(const vec4& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	return *this;
}

vec4& vec4::operator/=(const vec4& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
	return *this;
}

vec4& vec4::operator=(const vec4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

bool vec4::operator==(const vec4& v) const
{
	return x==v.x && y==v.y && z==v.z && w==v.w;
}

float vec4::mag() const
{
	return sqrtf(x*x+y*y+z*z+w*w);
}

vec4 vec4::normal() const
{
	return *this / mag();
}

vec4& vec4::normalize()
{
	return *this /= mag();
}

vec3 vec4::xyz()
{
	return vec3(x, y, z);
}
