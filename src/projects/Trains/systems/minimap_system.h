#ifndef PROJECTS_TRAINS_SYSTEMS_MINIMAP_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_MINIMAP_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

#include "projects/Trains/managers/minimap_manager.h"

namespace trains::systems {

class MinimapSystem : public core::ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;

private:
	trains::managers::MinimapManager& minimap_manager_ = trains::managers::MinimapManager::GetInstance();
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_MINIMAP_SYSTEM_H
