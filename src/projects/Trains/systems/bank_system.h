#ifndef TRAINS_SYSTEMS_BANK_SYSTEM_H
#define TRAINS_SYSTEMS_BANK_SYSTEM_H

#include "core/ecs/system.h"
#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"

namespace trains::systems {

class BankSystem : public core::ecs::System {
public:
	void Start() override;
	void StartArchetype(core::ecs::Archetype& archetype) override;
	void Tick(float delta_time) override;
	void TickArchetype(core::ecs::Archetype& archetype, float delta_time) override;
	
private:
	core::ecs::ECSManager& ecs_manager_ = core::ecs::ECSManager::GetInstance();
};
} // namespace trains::systems

#endif // TRAINS_SYSTEMS_BANK_SYSTEM_H
