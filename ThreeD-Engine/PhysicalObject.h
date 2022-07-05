#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
#include "FEA.h"

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
	PhysicalObject(std::string name, SceneSystem* system);

	~PhysicalObject();

	TetrahedralMeshPtr getTetrahedral() { return m_tetrahedral; }

	std::shared_ptr<FEA> getFEA() { return m_fea; }

	virtual SceneObjectType getType() const {
		return SceneObjectType::PhysicalObject;
	}

	inline EditableMeshPtr getMesh() { return m_mesh; }

	virtual ScriptValue* dot(std::string);
private:
	EditableMeshPtr m_mesh;
	TetrahedralMeshPtr m_tetrahedral;
	std::shared_ptr<FEA> m_fea;
};

