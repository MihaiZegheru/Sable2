#ifndef CORE_GRAPHICS_MODEL_H
#define CORE_GRAPHICS_MODEL_H

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

#include "material.h"
#include "mesh.h"

namespace core::graphics {

struct Model {
	struct MeshInstance {
		int mesh_index;
		int material_index;
		glm::mat4 transformation_matrix;
	};

	size_t id;
	std::vector<MeshInstance> mesh_instances;
	std::vector<Mesh> meshes;
	std::vector<Material> materials;

};
} // namespace core::graphics

#endif // CORE_GRAPHICS_MODEL_H