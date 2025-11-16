#include "entity_manager.h"

#include <expected>
#include <limits>
#include <string>

#include "types.h"

namespace Core {

EntityManager::EntityManager() {
	for (EntityID id = 0; id < std::numeric_limits<EntityID>::max(); ++id) {
		available_entities_.push(Entity(id));
	}
}

std::expected<Entity, std::string> EntityManager::CreateEntity() {
	if (available_entities_.empty()) {
		return std::unexpected("No more available entity IDs.");
	}
	Entity entity = available_entities_.front();
	available_entities_.pop();
	return entity;
}

void EntityManager::DestroyEntity(Entity entity) {
	available_entities_.push(entity);
}
} // namespace Core
