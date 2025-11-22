#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "archetype.h"

namespace core::ecs {

class System {
public:
	virtual ~System() = default;

	// Pure initialization function to be overridden by derived classes.
	virtual void Start() = 0;

	// Calls the initialization function for a specific archetype. This function should only be
	// called by the higher manager (SystemManager). It calls Start for each entity in the archetype.
	virtual void StartArchetype(core::ecs::Archetype& archetype) = 0;

	// Pure update function to be overridden by derived classes.
	virtual void Tick(float delta_time) = 0;

	// TODO: Add abstraction layer and simulate calling this per entity as Tick.

	// Calls the update function for a specific archetype. This function should only be called by
	// the higher manager (SystemManager). It calls Tick for each entity in the archetype.
	virtual void TickArchetype(Archetype& archetype, float delta_time) = 0;
};
} // namespace core::ecs

#endif // CORE_SYSTEM_H
