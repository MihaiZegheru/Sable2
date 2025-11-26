#include "collision_system.h"

#include <vector>

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"

#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/managers/collision_manager.h"

namespace trains::systems {

void CollisionSystem::Start() {

}

void CollisionSystem::StartAllArchetypes() {

}

void CollisionSystem::Tick(float delta_time) {

}

void CollisionSystem::TickAllArchetypes(float delta_time) {
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::BoxCollider>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
		archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
			trains::attributes::BoxCollider& box_collider = ecs_manager_.GetAttribute<trains::attributes::BoxCollider>(entity_id);
			collision_manager.AddCollidable(entity_id);
		});
	}
}
} // namespace trains::systems
