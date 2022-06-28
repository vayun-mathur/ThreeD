#pragma once

#include <memory>
#include <vector>

#include "Eigen/Dense"

#include "vector.h"

class TetrahedralMesh;

class FEA
{
public:
	FEA(std::shared_ptr<TetrahedralMesh> mesh, double total_mass, double dt);
	~FEA();

	Eigen::Matrix4d BuildBarycentricMatrix(vec3 x1, vec3 x2, vec3 x3, vec3 x4);

	Eigen::Matrix<double, 6, 12> BuildBe(Eigen::Matrix4d Pe);

	Eigen::Matrix<double, 12, 12> BuildKe(vec3 x1, vec3 x2, vec3 x3, vec3 x4, double mu, double lambda);

	void AssembleGlobalStiffnessMatrix(double mu, double lambda);

	void CreateFloatingNodeStiffnessMatrix();

	void CreateLumpedMassMatrix(int num_nodes, double total_mass);

	Eigen::MatrixXd BuildDampingMatrix(int num_nodes, double damping);

	void Update();

	// TODO: should allow for setting a force on multiple nodes, in one method
	void SetForce(int node_index, vec3 force);

private:
	std::shared_ptr<TetrahedralMesh> m_mesh;

	Eigen::MatrixXd m_stiffness_matrix;

	Eigen::MatrixXd m_floating_node_stiffness_matrix;

	Eigen::MatrixXd m_lumped_mass_matrix;

	Eigen::MatrixXd m_x0;

	// TODO Eigen::MatrixXd _x; // need to keep this for physics timestep update? or set this back onto the tetrahedron (eventually?)?
	// store here for now
	Eigen::MatrixXd m_x;

	Eigen::MatrixXd m_v;

	// (is S matrix in PBR)
	Eigen::MatrixXd m_A;

	double m_delta_time;

	int m_xdim;

	Eigen::MatrixXd m_force;

	std::vector<int> m_original_node_indices_mapped_to_non_fixed;
};

