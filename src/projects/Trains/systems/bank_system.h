#ifndef TRAINS_SYSTEMS_BANK_SYSTEM_H
#define TRAINS_SYSTEMS_BANK_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

#include "projects/Trains/attributes/bank.h"
#include "projects/Trains/managers/map_manager.h"

namespace trains::systems {

class BankSystem : public core::ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;

private:
	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();
};
} // namespace trains::systems

#endif // TRAINS_SYSTEMS_BANK_SYSTEM_H
