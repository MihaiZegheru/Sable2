#ifndef CORE_RENDER_RENDER_MATERIAL_DATA_H
#define CORE_RENDER_RENDER_MATERIAL_DATA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace core::render {

struct RenderMaterialData {
	GLuint diffuseTexture;
	GLuint normalMap;
	glm::vec4 baseColor;
	int textureMask;
};
} // namespace core::render

#endif // CORE_RENDER_RENDER_MATERIAL_DATA_H
