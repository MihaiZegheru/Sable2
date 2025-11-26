#ifndef PROJECTS_TRAINS_ATTRIBUTES_MINIMAP_RENDERABLE_H
#define PROJECTS_TRAINS_ATTRIBUTES_MINIMAP_RENDERABLE_H

#include <glm/vec3.hpp>

#include "core/ecs/types.h"

namespace trains::attributes {

struct MinimapRenderable : core::ecs::IAttribute {
	uint16_t model_id;
	glm::vec3 color;
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_MINIMAP_RENDERABLE_H
