#ifndef CORE_GRAPHICS_MESH_H
#define CORE_GRAPHICS_MESH_H

#include <cstddef>
#include <vector>

#include "vertex.h"

namespace core::graphics {

struct Mesh {
	unsigned int index;
	unsigned int vertices_count;
	unsigned int faces_count;
	std::vector<Vertex> vertices;
	std::vector <unsigned int> indices;
};
} // namespace core::graphics

#endif // CORE_GRAPHICS_MESH_H