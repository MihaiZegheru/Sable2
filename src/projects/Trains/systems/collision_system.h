#ifndef PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/managers/collision_manager.h"

namespace trains::systems {

class CollisionSystem : public core::ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;

private:
	trains::managers::CollisionManager& collision_manager = trains::managers::CollisionManager::GetInstance();
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H
