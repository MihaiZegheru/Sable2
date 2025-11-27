#ifndef CORE_ATTRIBUTES_CAMERA_H
#define CORE_ATTRIBUTES_CAMERA_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

#include "core/ecs/types.h"
#include "renderable.h"

namespace core::attributes {

enum class CameraScope {
	kScene,
	kUI,
	kTexture
};

enum class CameraType {
	kPerspective,
	kOrthographic
};

struct Camera : IRenderable {
	CameraType type = CameraType::kPerspective;

	// Perspective parameters
	float fov = 45.0f;
	float near_plane = 0.1f;
	float far_plane = 1000.0f;

	// Orthographic parameters
	float ortho_size = 10.0f;
	float ortho_width = 800.0f;
	float ortho_height = 600.0f;

	int width = 800;
	int height = 600;

	GLuint fbo = 0; 
	GLuint color = 0;
	GLuint depth = 0;

	CameraScope scope = CameraScope::kScene;

	// TODO: look_at defaults to id 0, which may be actually be a valid entity.

	ecs::EntityID look_at = 0;

	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_CAMERA_H
