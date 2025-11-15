#ifndef CORE_ENTITY_MANAGER_H
#define CORE_ENTITY_MANAGER_H

#include <expected>
#include <queue>
#include <string>

#include "entity.h"

namespace Core {

// Manages entity creation and recycling.
class EntityManager {
public:
	EntityManager();

	// Create a new entity and return its ID.
	std::expected<Entity, std::string> CreateEntity();
	// Destroy an entity, making its ID available for reuse.
	void DestroyEntity(Entity entity);

private:
	// Queue to hold available entity IDs
	std::queue<Entity> available_entities_;
};
} // namespace Core

#endif // CORE_ENTITY_MANAGER_H
