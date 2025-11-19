#ifndef CORE_ENTITY_MANAGER_H
#define CORE_ENTITY_MANAGER_H

#include <array>
#include <cassert>
#include <expected>
#include <queue>
#include <string>

#include "entity.h"
#include "types.h"

namespace core::ecs {

constexpr size_t kMaxEntities = 1 << 16;

// Manages entity creation and recycling.
class EntityManager {
public:
	explicit EntityManager();

	// Create a new entity and return its ID.
	std::expected<Entity, std::string> CreateEntity();
	// Destroy an entity, making its ID available for reuse.
	void DestroyEntity(EntityID entity);

	inline ArchetypeSignature GetEntitySignature(EntityID entity) const {
		assert(entity < kMaxEntities && "Entity ID out of bounds");
		return signatures_[entity];
	}
	inline void SetEntitySignature(EntityID entity, const ArchetypeSignature& signature) {
		assert(entity < kMaxEntities && "Entity ID out of bounds");
		signatures_[entity] = signature;
	}

private:
	// Queue to hold available entity IDs
	std::queue<Entity> available_entities_;
	// Array of signatures (one per entity)
    std::array<ArchetypeSignature, kMaxEntities> signatures_;
};
} // namespace core::ecs

#endif // CORE_ENTITY_MANAGER_H
