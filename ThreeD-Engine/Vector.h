#pragma once
#include <memory>

class vec2 {
public:
	vec2() : x(0), y(0) {}
	vec2(float x, float y) : x(x), y(y) {}
	vec2(const vec2& vector) : x(vector.x), y(vector.y) {}

	static vec2 lerp(const vec2& start, const vec2& end, float delta) {
		vec2 v;
		v.x = start.x * (1.0f - delta) + end.x * delta;
		v.y = start.y * (1.0f - delta) + end.y * delta;
		return v;
	}

	vec2 operator*(float scalar)
	{
		return vec2(x * scalar, y * scalar);
	}

	vec2 operator+(const vec2& other)
	{
		return vec2(x + other.x, y + other.y);
	}

	~vec2() {

	}
public:
	float x, y;
};

class vec3 {
public:
	vec3() : x(0), y(0), z(0) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	vec3(const vec3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

	static vec3 lerp(const vec3& start, const vec3& end, float delta) {
		vec3 v;
		v.x = start.x * (1.0f - delta) + end.x * delta;
		v.y = start.y * (1.0f - delta) + end.y * delta;
		v.z = start.z * (1.0f - delta) + end.z * delta;
		return v;
	}

	vec3 operator*(float scalar)
	{
		return vec3(x * scalar, y * scalar, z * scalar);
	}

	vec3 operator+(const vec3& other)
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	vec3 operator-(const vec3& other)
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}

	vec3 norm() const {
		float mag = sqrt(x * x + y * y + z * z);
		return vec3(x / mag, y / mag, z / mag);
	}

	~vec3() {

	}
public:
	float x, y, z;
};

class vec4 {
public:
	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	vec4(const vec4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}
	vec4(const vec3& vector) : x(vector.x), y(vector.y), z(vector.z), w(1) {}

	void cross(vec4& v1, vec4& v2, vec4& v3)
	{
		this->x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
		this->y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
		this->z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
		this->w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));
	}

	~vec4() {

	}
public:
	float x, y, z, w;
};


class mat4 {
public:
	mat4() {
		setIdentity();
	}

	void setIdentity()
	{
		::memset(mat, 0, sizeof(float) * 16);
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
		mat[3][3] = 1;
	}

	void setTranslation(const vec3& translation)
	{
		mat[3][0] = translation.x;
		mat[3][1] = translation.y;
		mat[3][2] = translation.z;
	}

	void setScale(const vec3& scale) {
		mat[0][0] = scale.x;
		mat[1][1] = scale.y;
		mat[2][2] = scale.z;
	}

	void setRotationX(float x)
	{
		mat[1][1] = cosf(x);
		mat[1][2] = sinf(x);
		mat[2][1] = -sinf(x);
		mat[2][2] = cosf(x);
	}

	void setRotationY(float y)
	{
		mat[0][0] = cosf(y);
		mat[0][2] = -sinf(y);
		mat[2][0] = sinf(y);
		mat[2][2] = cosf(y);
	}

	void setRotationZ(float z)
	{
		mat[0][0] = cosf(z);
		mat[0][1] = sinf(z);
		mat[1][0] = -sinf(z);
		mat[1][1] = cosf(z);
	}

	float getDeterminant()
	{
		vec4 minor, v1, v2, v3;
		float det;

		v1 = vec4(this->mat[0][0], this->mat[1][0], this->mat[2][0], this->mat[3][0]);
		v2 = vec4(this->mat[0][1], this->mat[1][1], this->mat[2][1], this->mat[3][1]);
		v3 = vec4(this->mat[0][2], this->mat[1][2], this->mat[2][2], this->mat[3][2]);


		minor.cross(v1, v2, v3);
		det = -(this->mat[0][3] * minor.x + this->mat[1][3] * minor.y + this->mat[2][3] * minor.z +
			this->mat[3][3] * minor.w);
		return det;
	}

	void inverse()
	{
		int a, i, j;
		mat4 out;
		vec4 v, vec[3];
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].x = (this->mat[j][0]);
					vec[a].y = (this->mat[j][1]);
					vec[a].z = (this->mat[j][2]);
					vec[a].w = (this->mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);

			out.mat[0][i] = (i & 1 ? -1.0f : 1.0f) * v.x / det;
			out.mat[1][i] = (i & 1 ? -1.0f : 1.0f) * v.y / det;
			out.mat[2][i] = (i & 1 ? -1.0f : 1.0f) * v.z / det;
			out.mat[3][i] = (i & 1 ? -1.0f : 1.0f) * v.w / det;
		}

		this->setMatrix(out);
	}

	void setMatrix(const mat4& matrix)
	{
		::memcpy(mat, matrix.mat, sizeof(float) * 16);
	}

	vec3 getZDirection()
	{
		return vec3(mat[2][0], mat[2][1], mat[2][2]);
	}

	vec3 getXDirection()
	{
		return vec3(mat[0][0], mat[0][1], mat[0][2]);
	}

	vec3 getTranslation()
	{
		return vec3(mat[3][0], mat[3][1], mat[3][2]);
	}

	mat4& operator *=(const mat4& matrix)
	{
		mat4 out;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out.mat[i][j] =
					mat[i][0] * matrix.mat[0][j]
					+ mat[i][1] * matrix.mat[1][j]
					+ mat[i][2] * matrix.mat[2][j]
					+ mat[i][3] * matrix.mat[3][j];
			}
		}
		setMatrix(out);
		return *this;
	}

	void setPerspectiveFovLH(float fov, float aspect, float znear, float zfar)
	{
		float yscale = 1.0f / tanf(fov / 2.0f);
		float xscale = yscale / aspect;
		mat[0][0] = xscale;
		mat[1][1] = yscale;
		mat[2][2] = zfar / (zfar - znear);
		mat[2][3] = 1.0f;
		mat[3][2] = (-znear * zfar) / (zfar - znear);
	}

	void setOrthoLH(float width, float height, float near_plane, float far_plane)
	{
		setIdentity();
		mat[0][0] = 2.0f / width;
		mat[1][1] = 2.0f / height;
		mat[2][2] = 1.0f / (far_plane - near_plane);
		mat[3][2] = -(near_plane / (far_plane - near_plane));
	}

	~mat4()
	{

	}
private:
	float mat[4][4] = {};
};