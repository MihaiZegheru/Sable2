#include "ecs_manager.h"

#include <stdexcept>


namespace Core {

Entity ECSManager::CreateEntity() {
	auto entity_result = entity_manager_.CreateEntity();
	if (!entity_result.has_value()) {
		throw std::runtime_error("Failed to create entity: " + entity_result.error());
	}
	return entity_result.value();
}

void ECSManager::DestroyEntity(Entity entity) {
	archetype_manager_.RemoveEntity(entity.id);
	entity_manager_.DestroyEntity(entity);

	// TODO: Might want to remove from systems as well.
}
} // namespace Core
