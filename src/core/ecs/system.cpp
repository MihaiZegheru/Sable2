#include "system.h"

namespace core::ecs {

void System::TickArchetype(Archetype& archetype, float delta_time) {
	// archetype.ForEach([this, delta_time](EntityID entity_id, size_t index) {
	// 	this->Tick(delta_time);
	// });
}
} // namespace core::ecs