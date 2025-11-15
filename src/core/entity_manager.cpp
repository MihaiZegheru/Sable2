#include "entity_manager.h"

#include <expected>
#include <limits>
#include <string>

namespace Core {

EntityManager::EntityManager() {
	for (uint16_t id = 0; id < std::numeric_limits<uint16_t>::max(); ++id) {
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
