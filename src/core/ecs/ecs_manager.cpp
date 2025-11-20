#include "ecs_manager.h"

#include <stdexcept>

namespace core::ecs {

Entity ECSManager::CreateEntity() {
	auto entity_result = entity_manager_.CreateEntity();
	if (!entity_result.has_value()) {
		throw std::runtime_error("Failed to create entity: " + entity_result.error());
	}

	// Add new entity to default archetype.
	archetype_manager_.AddEntity(entity_result->id, ArchetypeSignature{});

	return entity_result.value();
}

void ECSManager::DestroyEntity(EntityID entity) {
	archetype_manager_.RemoveEntity(entity);
	entity_manager_.DestroyEntity(entity);

	// TODO: Might want to remove from systems as well.
}
} // namespace core::ecs
