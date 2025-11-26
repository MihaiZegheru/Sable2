#include "follow_system.h"

#include "core/attributes/transform.h"
#include "core/attributes/follow.h"

namespace core::systems {

void FollowSystem::Start() {
	// Initialization if needed
}

void FollowSystem::StartAllArchetypes() {
	// Initialization per archetype if needed
}

void FollowSystem::Tick(float delta_time) {
	// Tick
}

void FollowSystem::TickAllArchetypes(float delta_time) {
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<attributes::Follow,
										 attributes::Transform>());
	for (auto& archetype_ref : archetypes) {
		ecs::Archetype& archetype = archetype_ref.get();
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
}
} // namespace core::systems