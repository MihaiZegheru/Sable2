#ifndef PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/managers/collision_manager.h"

namespace trains::systems {

class CollisionSystem : public core::ecs::System {
public:
	void Start() override;
	void StartArchetype(core::ecs::Archetype& archetype) override;
	void Tick(float delta_time) override;
	void TickArchetype(core::ecs::Archetype& archetype, float delta_time) override;

private:
	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();
	trains::managers::CollisionManager& collision_manager = trains::managers::CollisionManager::GetInstance();
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_COLLISION_SYSTEM_H
