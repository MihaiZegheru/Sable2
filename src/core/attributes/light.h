#ifndef CORE_ATTRIBUTES_LIGHT_H
#define CORE_ATTRIBUTES_LIGHT_H

#include <glm/glm.hpp>

#include "renderable.h"

namespace core::attributes {

struct Light : IRenderable {
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 1.0f;
	float linear_attenuation = 0.0f;
	float quadratic_attenuation = 0.0f;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_LIGHT_H
