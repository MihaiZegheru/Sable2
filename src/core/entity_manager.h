#ifndef CORE_ENTITY_MANAGER_H
#define CORE_ENTITY_MANAGER_H

#include <array>
#include <cassert>
#include <expected>
#include <queue>
#include <string>

#include "entity.h"
#include "types.h"

namespace Core {
	
constexpr size_t kMaxEntities = 1 << 16;

// Manages entity creation and recycling.
class EntityManager {
public:
	explicit EntityManager();

	// Create a new entity and return its ID.
	std::expected<Entity, std::string> CreateEntity();
	// Destroy an entity, making its ID available for reuse.
	void DestroyEntity(Entity entity);

	inline ArchetypeSignature GetEntitySignature(Entity entity) const {
		assert(entity.id < kMaxEntities && "Entity ID out of bounds");
		return signatures_[entity.id];
	}
	inline void SetEntitySignature(Entity entity, const ArchetypeSignature& signature) {
		assert(entity.id < kMaxEntities && "Entity ID out of bounds");
		signatures_[entity.id] = signature;
	}

private:
	// Queue to hold available entity IDs
	std::queue<Entity> available_entities_;
	// Array of signatures (one per entity)
    std::array<ArchetypeSignature, kMaxEntities> signatures_;
};
} // namespace Core

#endif // CORE_ENTITY_MANAGER_H
