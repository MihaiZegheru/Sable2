#ifndef CORE_ENTITY_H
#define CORE_ENTITY_H

#include "types.h"

namespace Core {

class EntityManager;

// Represents an entity in the system. Entities are identified by a unique ID.
// Entities cannot be created directly; they must be created and managed by the EntityManager.
struct Entity {
	friend class Core::EntityManager;

	// Underlying ID of the entity.
	EntityID id;

private:
	explicit Entity(EntityID entity_id) : id(entity_id) {}
	Entity() = delete;
};
} // namespace Core

#endif // CORE_ENTITY_H
