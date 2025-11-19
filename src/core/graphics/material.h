#ifndef CORE_GRAPHICS_MATERIAL_H
#define CORE_GRAPHICS_MATERIAL_H

#include <glm/glm.hpp>

#include "texture.h"

namespace core::graphics {

struct Material {
	Texture diffuse_texture;
	Texture normal_map;
	glm::vec4 base_color;
	int texture_mask;
};
} // namespace core::graphics

#endif // CORE_GRAPHICS_MATERIAL_H
