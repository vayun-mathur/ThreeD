#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"

struct Force {
	vec3 force;
	vec3 position;
};

inline vec4 quaternion_multiply(vec4 a, vec4 b) {
	return vec4(
		a.x * b.x - a.y * b.y - a.z * b.z - a.w * b.w,
		a.x * b.y + a.y * b.x + a.z * b.w - a.w * b.z,
		a.x * b.z + a.z * b.x + a.w * b.y - a.y * b.w,
		a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x
	);
}

class PhysicalObject
	: public SceneObject
{
public:
	PhysicalObject(std::string name, SceneSystem* system, std::wstring mesh_location, float mass);

	~PhysicalObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::PhysicalObject;
	}

	inline MeshPtr getMesh() { return m_mesh; }

	virtual ScriptValue* dot(std::string);
private:
	MeshPtr m_mesh;

public:
	inline vec3 getLinearPosition() { return m_lin_position; }
	inline void setLinearPosition(vec3 linear_position) { m_lin_position = linear_position; }
	inline vec3 getLinearMomentum() { return m_lin_momentum; }
	inline void setLinearMomentum(vec3 linear_momentum) { m_lin_momentum = linear_momentum; }
	inline vec3 getLinearVelocity() { return m_lin_momentum/ lin_inertia; }
	inline void setLinearVelocity(vec3 linear_velocity) { m_lin_momentum = linear_velocity * lin_inertia; }

	inline vec4 getAngularPosition() { return m_ang_position; }
	inline void setAngularPosition(vec4 angular_position) { m_ang_position = angular_position; }
	inline vec3 getAngularMomentum() { return m_ang_momentum; }
	inline void setAngularMomentum(vec3 angular_momentum) { m_ang_momentum = angular_momentum; }
	inline vec3 getAngularVelocity() { mat3 d = ang_inertia; return d.inverse()(m_ang_momentum); }
	inline void setAngularVelocity(vec3 angular_velocity) { m_ang_momentum = ang_inertia(angular_velocity); }

	void update(std::vector<Force> forces, float dt) {
		for (Force& f : forces) {
			m_lin_momentum += f.force * dt;
			m_ang_momentum += vec3::cross(f.position - m_lin_position, f.force) * dt;
		}

		m_lin_position += getLinearVelocity() * dt;
		m_ang_position += quaternion_multiply(vec4(0, getAngularVelocity()), m_ang_position) * 0.5 * dt;
		m_ang_position.normalize();
	}
private:
	float lin_inertia;
	mat3 ang_inertia;

	vec3 m_lin_position = vec3(0, 0, 0);
	vec3 m_lin_momentum;
	vec4 m_ang_position = vec4(1, 0, 0, 0);
	vec3 m_ang_momentum;
};

