#pragma once

#include <memory>
#include <vector>

#include "Eigen/Dense"

#include "vector.h"

class TetrahedralMesh;

class FEA
{
public:
	FEA(std::shared_ptr<TetrahedralMesh> mesh, float total_mass, float dt);
	~FEA();

	Eigen::Matrix4f BuildBarycentricMatrix(vec3 x1, vec3 x2, vec3 x3, vec3 x4);

	Eigen::Matrix<float, 6, 12> BuildBe(Eigen::Matrix4f Pe);

	Eigen::Matrix<float, 12, 12> BuildKe(vec3 x1, vec3 x2, vec3 x3, vec3 x4, float mu, float lambda);

	void AssembleGlobalStiffnessMatrix(float mu, float lambda);

	void CreateFloatingNodeStiffnessMatrix();

	void CreateLumpedMassMatrix(int num_nodes, float total_mass);

	Eigen::MatrixXf BuildDampingMatrix(int num_nodes, float damping);

	void Update();

	void ResetForces();

	// TODO: should allow for setting a force on multiple nodes, in one method
	void SetForce(int node_index, vec3 force);

private:
	std::shared_ptr<TetrahedralMesh> m_mesh;

	Eigen::MatrixXf m_stiffness_matrix;

	Eigen::MatrixXf m_floating_node_stiffness_matrix;

	Eigen::MatrixXf m_lumped_mass_matrix;

	Eigen::MatrixXf m_x0;

	// TODO Eigen::MatrixXd _x; // need to keep this for physics timestep update? or set this back onto the tetrahedron (eventually?)?
	// store here for now
	Eigen::MatrixXf m_x;

	Eigen::MatrixXf m_v;

	// (is S matrix in PBR)
	Eigen::MatrixXf m_A;

	float m_delta_time;

	int m_xdim;

	Eigen::MatrixXf m_force;

	std::vector<int> m_original_node_indices_mapped_to_non_fixed;
};

