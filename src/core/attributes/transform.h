#ifndef CORE_ATTRIBUTES_TRANSFORM_H
#define CORE_ATTRIBUTES_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "core/ecs/types.h"

namespace core::attributes {

struct Transform : ecs::IAttribute {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform()
	    : position(0.0f, 0.0f, 0.0f),
	      rotation(1.0f, 0.0f, 0.0f, 0.0f),
	      scale(1.0f, 1.0f, 1.0f) {}

	glm::mat4 GetModelMatrix() const {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 rotation_mat = glm::mat4_cast(rotation);
		glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);
		return translation * rotation_mat * scale_mat;
	}
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_TRANSFORM_H
