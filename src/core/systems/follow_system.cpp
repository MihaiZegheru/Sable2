#include "follow_system.h"

#include "core/attributes/transform.h"
#include "core/attributes/follow.h"

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
		attributes::Follow& follow = ecs_manager_.GetAttribute<attributes::Follow>(entity_id);
		attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
		attributes::Transform& target_transform = ecs_manager_.GetAttribute<attributes::Transform>(follow.target_entity);

		transform.position = target_transform.position + follow.offset;

		if (follow.match_rotation) {
			// TODO: Add
		}
	});
} 
} // namespace core::systems