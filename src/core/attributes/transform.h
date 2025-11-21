#ifndef CORE_ATTRIBUTES_TRANSFORM_H
#define CORE_ATTRIBUTES_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "core/ecs/types.h"

namespace core::attributes {

struct Transform : ecs::IAttribute {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform()
	    : position(0.0f, 0.0f, 0.0f),
	      rotation(0.0f, 0.0f, 0.0f),
	      scale(1.0f, 1.0f, 1.0f) {}

	// TODO: Should move to some other attribute and add dirty flagging
	
	glm::mat4 GetModelMatrix() const {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 rotation_mat = glm::yawPitchRoll(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
		glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);
		return translation * rotation_mat * scale_mat;
	}
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_TRANSFORM_H
