#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "archetype.h"
#include "archetype_manager.h"

namespace core::ecs {

class ECSManager;

class ISystem {
public:
	ISystem();

	// Pure initialization function to be overridden by derived classes.
	virtual void Start() = 0;

	// Calls the initialization function for a specific archetype. This function should only be
	// called by the higher manager (SystemManager). It calls Start for each entity in the archetype.
	virtual void StartAllArchetypes() = 0;

	// Pure update function to be overridden by derived classes.
	virtual void Tick(float delta_time) = 0;

	// Calls the update function for a specific archetype. This function should only be called by
	// the higher manager (SystemManager). It calls Tick for each entity in the archetype.
	virtual void TickAllArchetypes(float delta_time) = 0;

protected:
	ECSManager& ecs_manager_;
	ArchetypeManager& archetype_manager_;
};
} // namespace core::ecs

#endif // CORE_SYSTEM_H
