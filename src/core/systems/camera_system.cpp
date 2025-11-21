#include "camera_system.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "core/attributes/camera.h"
#include "core/attributes/transform.h"


namespace core::systems {

void CameraSystem::Tick(float delta_time) {
	// Tick
}

void CameraSystem::TickArchetype(ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](ecs::EntityID entity_id, size_t index) {
		attributes::Camera& camera = ecs_manager_.GetAttribute<attributes::Camera>(entity_id);
		attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);

		// Update view matrix based on transform
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -transform.position);
		glm::mat4 rotation = glm::yawPitchRoll(glm::radians(-transform.rotation.y), glm::radians(-transform.rotation.x), glm::radians(-transform.rotation.z));
		camera.view_matrix = rotation * translation;

		// Update projection matrix based on FOV and aspect ratio (assuming 16:9 here)
		float aspect_ratio = 16.0f / 9.0f;
		camera.projection_matrix = glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane);
	});
}
} // namespace core::systems
