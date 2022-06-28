#pragma once
#include <memory>
#undef min
#undef max

class vec2 {
public:
	vec2();
	vec2(float f);
	vec2(float x, float y);
	vec2(const vec2& vector);

	static vec2 lerp(const vec2& start, const vec2& end, float delta);
	static vec2 lerp_bounds(const vec2& start, const vec2& end, float delta_start, float delta_end, float delta);
	static float dot(const vec2& v1, const vec2& v2);
	static float angle(const vec2& v1, const vec2& v2);

	vec2 operator+(const vec2& v) const;
	vec2 operator-(const vec2& v) const;
	vec2 operator*(const vec2& v) const;
	vec2 operator/(const vec2& v) const;

	vec2& operator+=(const vec2& v);
	vec2& operator-=(const vec2& v);
	vec2& operator*=(const vec2& v);
	vec2& operator/=(const vec2& v);

	vec2& operator=(const vec2& v);

	bool operator==(const vec2& v) const;

	float mag() const;
	vec2 normal() const;
	vec2& normalize();

public:
	union {
		struct {
			float x, y;
		};
		float m[2];
	};
};

class vec3 {
public:
	vec3();
	vec3(float f);
	vec3(float x, float y, float z);
	vec3(const vec2& xy, float z);
	vec3(float x, const vec2& yz);
	vec3(const vec3& vector);

	static vec3 lerp(const vec3& start, const vec3& end, float delta);
	static vec3 lerp_bounds(const vec3& start, const vec3& end, float delta_start, float delta_end, float delta);
	static float dot(const vec3& v1, const vec3& v2);
	static vec3 cross(const vec3& v1, const vec3& v2);
	static float angle(const vec3& v1, const vec3& v2);

	vec3 operator+(const vec3& v) const;
	vec3 operator-(const vec3& v) const;
	vec3 operator*(const vec3& v) const;
	vec3 operator/(const vec3& v) const;

	vec3& operator+=(const vec3& v);
	vec3& operator-=(const vec3& v);
	vec3& operator*=(const vec3& v);
	vec3& operator/=(const vec3& v);

	vec3& operator=(const vec3& v);

	bool operator==(const vec3& v) const;

	float mag() const;
	vec3 normal() const;
	vec3& normalize();

	vec2 xy();
	vec2 yz();
	vec2 xz();
	vec2 yx();
	vec2 zy();
	vec2 zx();

public:
	union {
		struct {
			float x, y, z;
		};
		float m[3];
	};
};

class vec4 {
public:
	vec4();
	vec4(float f);
	vec4(float x, float y, float z, float w);
	vec4(const vec2& xy, const vec2& zw);
	vec4(const vec2& xy, float z, float w);
	vec4(float x, float y, const vec2& zw);
	vec4(const vec3& xyz, float w);
	vec4(float x, const vec3& yzw);
	vec4(const vec4&& vector) noexcept;
	vec4(const vec4& vector);

	static vec4 lerp(const vec4& start, const vec4& end, float delta);
	static vec4 lerp_bounds(const vec4& start, const vec4& end, float delta_start, float delta_end, float delta);
	static float dot(const vec4& v1, const vec4& v2);
	static vec4 cross(const vec4& v1, const vec4& v2, const vec4& v3);
	static float angle(const vec4& v1, const vec4& v2);

	vec4 operator+(const vec4& v) const;
	vec4 operator-(const vec4& v) const;
	vec4 operator*(const vec4& v) const;
	vec4 operator/(const vec4& v) const;

	vec4& operator+=(const vec4& v);
	vec4& operator-=(const vec4& v);
	vec4& operator*=(const vec4& v);
	vec4& operator/=(const vec4& v);

	vec4& operator=(const vec4& v);

	bool operator==(const vec4& v) const;

	float mag() const;
	vec4 normal() const;
	vec4& normalize();

	vec3 xyz();
	//TODO: add rest of subvectors

	vec2 xy();
	vec2 yz();
	vec2 xz();
	vec2 yx();
	vec2 zy();
	vec2 zx();
	vec2 zw();

public:
	union {
		struct {
			float x, y, z, w;
		};
		float m[4];
	};
};



inline vec3 min(vec3 v1, vec3 v2) {
	return vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}
inline vec3 max(vec3 v1, vec3 v2) {
	return vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}


class mat3 {
public:
	mat3() {
		setIdentity();
	}
	mat3(vec3 x, vec3 y, vec3 z) {
		mat[0][0] = x.x;
		mat[0][1] = x.y;
		mat[0][2] = x.z;
		mat[1][0] = y.x;
		mat[1][1] = y.y;
		mat[1][2] = y.z;
		mat[2][0] = z.x;
		mat[2][1] = z.y;
		mat[2][2] = z.z;
	}

	void setIdentity()
	{
		::memset(mat, 0, sizeof(float) * 9);
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
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

	mat3 transposed() {
		mat3 o;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				o.mat[j][i] = mat[i][j];
			}
		}
		return o;
	}

	mat3 operator+(const mat3& other) {
		mat3 o;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				o.mat[i][j] = mat[i][j] + other.mat[i][j];
			}
		}
		return o;
	}

	mat3 operator*(float other) {
		mat3 o;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				o.mat[i][j] = mat[i][j] * other;
			}
		}
		return o;
	}

	float getDeterminant()
	{
		float x = mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2];
		float y = mat[1][0] * mat[2][2] - mat[2][0] * mat[1][2];
		float z = mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1];

		return (mat[0][0] * x - mat[0][1] * y + mat[0][2] * z);
	}

	mat3& inverse() {
		double det = getDeterminant();
		if (!det) return *this;
		double invdet = 1 / det;
		mat3 out;
		out.mat[0][0] = (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) * invdet;
		out.mat[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) * invdet;
		out.mat[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * invdet;
		out.mat[1][0] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) * invdet;
		out.mat[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * invdet;
		out.mat[1][2] = (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]) * invdet;
		out.mat[2][0] = (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) * invdet;
		out.mat[2][1] = (mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1]) * invdet;
		out.mat[2][2] = (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) * invdet;
		this->setMatrix(out);
		return *this;
	}

	void setMatrix(const mat3& matrix)
	{
		::memcpy(mat, matrix.mat, sizeof(float) * 9);
	}

	vec3 getZDirection()
	{
		return vec3(mat[2][0], mat[2][1], mat[2][2]);
	}

	vec3 getXDirection()
	{
		return vec3(mat[0][0], mat[0][1], mat[0][2]);
	}

	vec3 getYDirection()
	{
		return vec3(mat[1][0], mat[1][1], mat[1][2]);
	}

	mat3 operator()(const mat3& matrix)
	{
		mat3 out;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				out.mat[j][i] =
					mat[0][i] * matrix.mat[j][0]
					+ mat[1][i] * matrix.mat[j][1]
					+ mat[2][i] * matrix.mat[j][2];
			}
		}
		return out;
	}

	vec3 operator()(const vec3& vector)
	{
		vec3 out;
		for (int i = 0; i < 3; i++) {
			out.m[i] =
				mat[0][i] * vector.m[0]
				+ mat[1][i] * vector.m[1]
				+ mat[2][i] * vector.m[2];
		}
		return out;
	}

	~mat3()
	{

	}
private:
	float mat[3][3] = {};
};


class mat4 {
public:
	mat4() {
		setIdentity();
	}

	mat4(vec4 Q) {
		setIdentity();
		float q0 = Q.m[0];
		float q1 = Q.m[1];
		float q2 = Q.m[2];
		float q3 = Q.m[3];

		mat[0][0] = 2 * (q0 * q0 + q1 * q1) - 1;
		mat[0][1] = 2 * (q1 * q2 - q0 * q3);
		mat[0][2] = 2 * (q1 * q3 + q0 * q2);

		mat[1][0] = 2 * (q1 * q2 + q0 * q3);
		mat[1][1] = 2 * (q0 * q0 + q2 * q2) - 1;
		mat[1][2] = 2 * (q2 * q3 - q0 * q1);

		mat[2][0] = 2 * (q1 * q3 - q0 * q2);
		mat[2][1] = 2 * (q2 * q3 + q0 * q1);
		mat[2][2] = 2 * (q0 * q0 + q3 * q3) - 1;
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

		minor = vec4::cross(v1, v2, v3);
		det = -(this->mat[0][3] * minor.x + this->mat[1][3] * minor.y + this->mat[2][3] * minor.z +
			this->mat[3][3] * minor.w);
		return det;
	}

	mat4& inverse()
	{
		int a, i, j;
		mat4 out;
		vec4 v, vec[3];
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return *this;
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
			v = vec4::cross(vec[0], vec[1], vec[2]);

			out.mat[0][i] = (i & 1 ? -1.0f : 1.0f) * v.x / det;
			out.mat[1][i] = (i & 1 ? -1.0f : 1.0f) * v.y / det;
			out.mat[2][i] = (i & 1 ? -1.0f : 1.0f) * v.z / det;
			out.mat[3][i] = (i & 1 ? -1.0f : 1.0f) * v.w / det;
		}

		this->setMatrix(out);
		return *this;
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

	mat4 operator()(const mat4& matrix)
	{
		mat4 out;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out.mat[j][i] =
					mat[0][i] * matrix.mat[j][0]
					+ mat[1][i] * matrix.mat[j][1]
					+ mat[2][i] * matrix.mat[j][2]
					+ mat[3][i] * matrix.mat[j][3];
			}
		}
		return out;
	}

	vec4 operator()(const vec4& vector)
	{
		vec4 out;
		for (int i = 0; i < 4; i++) {
			out.m[i] =
				mat[0][i] * vector.m[0]
				+ mat[1][i] * vector.m[1]
				+ mat[2][i] * vector.m[2]
				+ mat[3][i] * vector.m[3];
		}
		return out;
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