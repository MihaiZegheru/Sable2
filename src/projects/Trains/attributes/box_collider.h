#ifndef PROJECTS_TRAINS_ATTRIBUTES_BOX_COLLIDER_H
#define PROJECTS_TRAINS_ATTRIBUTES_BOX_COLLIDER_H

#include <glm/vec3.hpp>

#include "core/ecs/types.h"

namespace trains::attributes {

struct BoxCollider : public core::ecs::IAttribute {
	glm::vec3 size;

	BoxCollider() : size{1.0f, 1.0f, 1.0f} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_BOX_COLLIDER_H
