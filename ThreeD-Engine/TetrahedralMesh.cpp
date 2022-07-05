#include "TetrahedralMesh.h"
#include <array>

// NOTE: hardcoded to create a 'wall' of cube, where each cube is formed of two 
//  triangular pyramids, each made from three tetrahedra
// would obviously be better to have the mesh definition stored externally in a file
TetrahedralMesh::TetrahedralMesh(int x_num, int y_num, int z_num, float cube_size)
{

	auto x_dim = (x_num + 1);
	auto y_dim = (y_num + 1);
	auto z_dim = (z_num + 1);

	auto xOffset = static_cast<float>(-(x_num * cube_size) / 2.0);

	for (auto x = 0; x < x_dim; ++x)
	{
		for (auto y = 0; y < y_dim; ++y)
		{
			for (auto z = 0; z < z_dim; ++z)
			{
				m_nodes.push_back(vec3(xOffset + (cube_size * x), cube_size * y, cube_size * z));
			}
		}
	}

	for (auto x = 0; x < x_dim; ++x)
	{
		for (auto z = 0; z < z_dim; ++z)
		{
			auto index = (x * (y_dim * z_dim)) + z;
			// The bottom of the wall is fixed
			m_fixed_nodes.push_back(index);
		}
	}

	for (auto x = 0; x < x_num; ++x)
	{
		for (auto y = 0; y < y_num; ++y)
		{
			for (auto z = 0; z < z_num; ++z)
			{
				auto bottom_left_front_index = (x * (y_dim * z_dim)) + (y * z_dim) + z;
				auto bottom_right_front_index = ((x + 1) * (y_dim * z_dim)) + (y * z_dim) + z;
				auto bottom_left_back_index = (x * (y_dim * z_dim)) + (y * z_dim) + (z + 1);
				auto top_left_front_index = (x * (y_dim * z_dim)) + ((y + 1) * z_dim) + z;
				auto top_right_front_index = ((x + 1) * (y_dim * z_dim)) + ((y + 1) * z_dim) + z;
				auto top_left_back_index = (x * (y_dim * z_dim)) + ((y + 1) * z_dim) + (z + 1);
				auto bottom_right_back_index = ((x + 1) * (y_dim * z_dim)) + (y * z_dim) + (z + 1);
				auto top_right_back_index = ((x + 1) * (y_dim * z_dim)) + ((y + 1) * z_dim) + (z + 1);

				auto triangular_prism_nodes1 = std::vector<int>{ bottom_left_front_index, bottom_right_front_index, bottom_left_back_index,
															   top_left_front_index, top_right_front_index, top_left_back_index };
				auto triangular_prism_nodes2 = std::vector<int>{ bottom_right_back_index, bottom_left_back_index, bottom_right_front_index,
															   top_right_back_index, top_left_back_index, top_right_front_index };

				auto triangular_prism_elements1 = GetTriangularPrismFromNodes(triangular_prism_nodes1);
				auto triangular_prism_elements2 = GetTriangularPrismFromNodes(triangular_prism_nodes2);
				m_elements.insert(end(m_elements), begin(triangular_prism_elements1), end(triangular_prism_elements1));
				m_elements.insert(end(m_elements), begin(triangular_prism_elements2), end(triangular_prism_elements2));
			}
		}
	}
}

std::vector<TetrahedralElementIndices> TetrahedralMesh::GetTriangularPrismFromNodes(std::vector<int> node_indices)
{
	auto output = std::vector<TetrahedralElementIndices>();

	output.push_back(TetrahedralElementIndices(node_indices[0], node_indices[2], node_indices[1], node_indices[3]));
	output.push_back(TetrahedralElementIndices(node_indices[3], node_indices[4], node_indices[5], node_indices[2]));
	output.push_back(TetrahedralElementIndices(node_indices[3], node_indices[1], node_indices[4], node_indices[2]));

	return output;
}

TetrahedralMesh::~TetrahedralMesh()
{
}

void TetrahedralMesh::SetNodePosition(int index, vec3 position)
{
	m_nodes[index] = position;
}

Mesh2 TetrahedralMesh::GetMesh()
{
	// NOTE: colours should also be stored in external file along with the vertices
	/*auto purple = vec3{ 1.0f, 0.0f, 1.0f };
	auto green = vec3{ 0.0f, 1.0f, 0.0f };
	auto red = vec3{ 1.0f, 0.0f, 0.0f };
	auto blue = vec3{ 0.0f, 0.0f, 1.0f };
	auto orange = vec3{ 1.0f, 1.0f, 0.0f };
	auto white = vec3{ 1.0f, 1.0f, 1.0f };

	auto colors = std::vector<vec3>{ purple, green, red, blue, orange, white };*/
	auto blue = vec3{ 0.0f, 0.0f, 1.0f };

	// duplicate points as have normals perpendicular to each face, not interpolated

	// TODO: use std::dynarray, or pass a stack allocator to vector

	std::vector<vec3> m_vertex_positions;
	m_vertex_positions.reserve(m_elements.size() * 12);

	std::vector<short> m_indices;
	m_indices.reserve(m_elements.size() * 12);

	for (auto element : m_elements)
	{
		auto node1 = m_nodes[element.a];
		auto node2 = m_nodes[element.b];
		auto node3 = m_nodes[element.c];
		auto node4 = m_nodes[element.d];

		// Now, have 4 triangles from this element
		// left-back-right-top refers to the tetrahedron nodes, assuming an equilateral tetrahedron with the 
		//  base triangle's base side perpendicular to the viewer
		auto left = node1;
		auto back = node3;
		auto right = node2;
		auto top = node4;

		m_vertex_positions.insert(end(m_vertex_positions),
			{
				// bottom
				left, right, back,
				// front
				left, right, top,
				// back-left
				left, back, top,
				// back-right
				right, back, top
			}
		);

		std::array<short, 12> elementTriangleIndices =
		{ {
				// bottom
				0, 2, 1,

				// front
				3, 4, 5,

				// back-left
				6, 8, 7,

				// back-right
				9, 10, 11
			} };

		auto offset = static_cast<int>(m_indices.size());

		for (auto index : elementTriangleIndices)
		{
			m_indices.push_back(index + offset);
		}
	}

	return Mesh2(m_vertex_positions, m_indices);
}