#include "collision_system.h"

#include <vector>

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"

#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/managers/collision_manager.h"

namespace trains::systems {

void CollisionSystem::Start() {

}

void CollisionSystem::StartArchetype(core::ecs::Archetype& archetype) {

}

void CollisionSystem::Tick(float delta_time) {
	
}

void CollisionSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::BoxCollider& box_collider = ecs_manager.GetAttribute<trains::attributes::BoxCollider>(entity_id);
		collision_manager.AddCollidable(entity_id);
	});
}
} // namespace trains::systems
