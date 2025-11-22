#ifndef CORE_SYSTEMS_FOLLOW_SYSTEM_H
#define CORE_SYSTEMS_FOLLOW_SYSTEM_H

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/follow.h"

namespace core::systems {

class FollowSystem : public ecs::System {
public:
	void Start() override;
	void StartArchetype(ecs::Archetype& archetype) override;
	void Tick(float delta_time) override;
	void TickArchetype(ecs::Archetype& archetype, float delta_time) override;
	
private:
	ecs::ECSManager& ecs_manager_ = ecs::ECSManager::GetInstance();
};
} // namespace core::systems

#endif // CORE_SYSTEMS_FOLLOW_SYSTEM_H
