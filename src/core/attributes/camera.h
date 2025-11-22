#ifndef CORE_ATTRIBUTES_CAMERA_H
#define CORE_ATTRIBUTES_CAMERA_H

#include <glm/glm.hpp>

#include "core/ecs/types.h"

namespace core::attributes {

struct Camera : ecs::IAttribute {
	float fov;
	float near_plane;
	float far_plane;

	// TODO: look_at defaults to id 0, which may be actually be a valid entity.

	ecs::EntityID look_at;

	Camera()
	    : fov(45.0f),
	      near_plane(0.1f),
	      far_plane(1000.0f),
	      look_at(0) {}

	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_CAMERA_H
