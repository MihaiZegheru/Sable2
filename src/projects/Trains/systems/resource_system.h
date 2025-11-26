#ifndef PROJECTS_TRAINS_SYSTEMS_RESOURCE_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_RESOURCE_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

namespace trains::systems {

class ResourceSystem : public core::ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_RESOURCE_SYSTEM_H
