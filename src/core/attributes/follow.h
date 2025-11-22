#ifndef CORE_ATTRIBUTES_FOLLOW_H
#define CORE_ATTRIBUTES_FOLLOW_H

#include <glm/vec3.hpp>

#include "core/ecs/types.h"

namespace core::attributes {

struct Follow : public core::ecs::IAttribute {
	core::ecs::EntityID target_entity;
	glm::vec3 offset;
	bool match_rotation;

	Follow() : target_entity(0), offset(10.0f, 10.0f, 10.0f), match_rotation(true) {}
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_FOLLOW_H
