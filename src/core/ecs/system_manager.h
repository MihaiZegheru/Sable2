#ifndef CORE_SYSTEM_MANAGER_H
#define CORE_SYSTEM_MANAGER_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "archetype.h"
#include "archetype_manager.h"
#include "system.h"
#include "types.h"

namespace core::ecs {

class SystemManager {
public:
	explicit SystemManager() = default;

	// Registers a system of type T with the given archetype signature.
	// T must be derived from System.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<System, T>>>
	void RegisterSystem(ArchetypeSignature& signature) {
		std::type_index type(typeid(T));

		assert(system_signatures_.find(type) == system_signatures_.end() &&
			   "System type already registered.");

		auto system = std::make_unique<T>();
		systems_.insert({type, std::move(system)});
		system_signatures_[type] = signature;
	}

	// Calls the Start function for all registered systems.
	void StartSystems();
	// Updates all registered systems by ticking their matching archetypes.
	void UpdateSystems(float delta_time);

	// Adds an archetype to all systems that match its signature.
	// Should be called whenever a new archetype is created.
	void OnArchetypeCreated(Archetype& archetype);
	// Rebuilds the cache of archetypes for all systems.
	// Should be called after all systems have been registered.
	void RebuildArchetypeCache(ArchetypeManager& archetype_manager);

private:
	// Maps system type to its instance. Stored as unique_ptr to manage lifetime.
	std::unordered_map<std::type_index, std::unique_ptr<System>> systems_;
	// Maps system type to its required archetype signature.
	std::unordered_map<std::type_index, ArchetypeSignature> system_signatures_;
    // Cache archetypes for each system for quick access during updates
    std::unordered_map<std::type_index, std::vector<std::reference_wrapper<Archetype>>> system_archetypes_;
};
} // namespace core::ecs

#endif // CORE_SYSTEM_MANAGER_H
