#ifndef PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H
#define PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/managers/input_manager.h"

#include "projects/Trains/attributes/train.h"
#include "projects/Trains/managers/map_manager.h"
#include "projects/Trains/managers/collision_manager.h"

namespace trains::systems {

class TrainSystem : public core::ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;

public:
	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();
	core::managers::InputManager& input_manager_ = core::managers::InputManager::GetInstance();
	trains::managers::CollisionManager& collision_manager_ = trains::managers::CollisionManager::GetInstance();
};
} // namespace trains::systems

#endif // PROJECTS_TRAINS_SYSTEMS_TRAIN_SYSTEM_H
