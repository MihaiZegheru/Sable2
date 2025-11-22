#include "follow_system.h"

#include "core/attributes/transform.h"
#include "core/attributes/follow.h"


#include <iostream>

namespace core::systems {

void FollowSystem::Start() {
	// Initialization if needed
}

void FollowSystem::StartArchetype(ecs::Archetype& archetype) {
	// Initialization per archetype if needed
}

void FollowSystem::Tick(float delta_time) {
	// Tick
}

void FollowSystem::TickArchetype(ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](ecs::EntityID entity_id, size_t index) {
		std::cout << "Ticking FollowSystem for entity " << entity_id << std::endl;
		attributes::Follow& follow = ecs_manager_.GetAttribute<attributes::Follow>(entity_id);
		attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
		std::cout << "Following entity " << follow.target_entity << std::endl;
		attributes::Transform& target_transform = ecs_manager_.GetAttribute<attributes::Transform>(follow.target_entity);
		std::cout << "CRASHED" << std::endl;

		transform.position = target_transform.position + follow.offset;

		if (follow.match_rotation) {
			transform.rotation = target_transform.rotation;
		}
		std::cout << "Updated position to (" << transform.position.x << ", "
				  << transform.position.y << ", " << transform.position.z << ")\n";
	});
} 
} // namespace core::systems