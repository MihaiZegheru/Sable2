#include "bank_system.h"

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "projects/Trains/attributes/bank.h"

namespace trains::systems {

void BankSystem::Start() {

}

void BankSystem::StartArchetype(core::ecs::Archetype& archetype) {

}

void BankSystem::Tick(float delta_time) {

}

void BankSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::Bank& bank = ecs_manager_.GetAttribute<trains::attributes::Bank>(entity_id);

	});
} 
} // namespace trains::systems
