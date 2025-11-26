#ifndef CORE_SYSTEMS_FOLLOW_SYSTEM_H
#define CORE_SYSTEMS_FOLLOW_SYSTEM_H

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/follow.h"

namespace core::systems {

class FollowSystem : public ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;
};
} // namespace core::systems

#endif // CORE_SYSTEMS_FOLLOW_SYSTEM_H
