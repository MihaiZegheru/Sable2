#ifndef PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "projects/Trains/attributes/train.h"
#include "projects/Trains/managers/map_manager.h"

namespace trains::systems {

class TrainSystem : public core::ecs::System {
public:
	void Start() override;
	void StartArchetype(core::ecs::Archetype& archetype) override;
	void Tick(float delta_time) override;
	void TickArchetype(core::ecs::Archetype& archetype, float delta_time) override;

public:
	core::ecs::ECSManager& ecs_manager_ = core::ecs::ECSManager::GetInstance();
	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H
