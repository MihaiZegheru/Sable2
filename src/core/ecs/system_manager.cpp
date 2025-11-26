#include "system_manager.h"

#include "archetype_manager.h"
#include "system.h"
#include "types.h"

namespace core::ecs {

void SystemManager::OnArchetypeCreated(Archetype& archetype) {
	ArchetypeSignature archetype_signature = archetype.GetSignature();
	for (auto& [type, system] : systems_) {
		ArchetypeSignature system_signature = system_signatures_[type];
		if ((archetype_signature & system_signature) == system_signature) {
			system_archetypes_[type].push_back(archetype);
		}
	}
}

void SystemManager::RebuildArchetypeCache(ArchetypeManager& archetype_manager) {
	system_archetypes_.clear();
	for (auto& [signature, archetype] : archetype_manager.GetAllArchetypes()) {
		for (auto& [type, system] : systems_) {
			auto const& system_signature = system_signatures_[type];
			if ((signature & system_signature) == system_signature) {
				system_archetypes_[type].push_back(*archetype);
			}
		}
	}
}

void SystemManager::StartSystems() {
	for (auto& [type, system] : systems_) {
		system->StartAllArchetypes();
	}
}

void SystemManager::UpdateSystems(float delta_time) {
	// TODO: Add system prioritization.

	for (auto& [type, system] : systems_) {
		system->TickAllArchetypes(delta_time);
	}
}
} // namespace core::ecs
