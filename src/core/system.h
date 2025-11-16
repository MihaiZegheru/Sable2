#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "archetype.h"

namespace Core {

class System {
public:
	virtual ~System() = default;

	// Pure update function to be overridden by derived classes.
	virtual void Tick(float delta_time) = 0;

	// Calls the update function for a specific archetype.
	void TickArchetype(Archetype& archetype, float delta_time);
};
} // namespace Core

#endif // CORE_SYSTEM_H
