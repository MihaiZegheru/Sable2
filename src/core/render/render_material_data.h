#ifndef CORE_RENDER_RENDER_MATERIAL_DATA_H
#define CORE_RENDER_RENDER_MATERIAL_DATA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "core/graphics/shader_program.h"
#include "core/managers/shader_manager.h"

namespace core::render {

struct RenderMaterialData {
	GLuint diffuseTexture;
	GLuint normalMap;
	glm::vec4 baseColor;
	int textureMask;

	graphics::ShaderProgram shader_program =
		managers::ShaderManager::GetInstance().GetDefaultShaderProgram();
};
} // namespace core::render

#endif // CORE_RENDER_RENDER_MATERIAL_DATA_H
