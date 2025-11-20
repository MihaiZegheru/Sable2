#ifndef CORE_RENDER_RENDER_MESH_DATA_H
#define CORE_RENDER_RENDER_MESH_DATA_H

#include <glad/glad.h>

namespace core::render {

struct RenderMeshData {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	int indicesSize;
	int materialIndex;
};
} // namespace core::render

#endif // CORE_RENDER_RENDER_MESH_DATA_H
