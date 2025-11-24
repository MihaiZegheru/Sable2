#ifndef PROJECTS_TRAINS_TYPES_COLLISION_H
#define PROJECTS_TRAINS_TYPES_COLLISION_H

#include "core/ecs/types.h"

namespace trains::types {

struct Collision {
	core::ecs::EntityID entityA;
	core::ecs::EntityID entityB;

	bool operator==(const Collision& other) const {
		return (entityA == other.entityA &&  entityB == other.entityB) ||
			   (entityA == other.entityB &&  entityB == other.entityA);
	}
};
} // namespace trains::types

namespace std {
	
template <>
struct hash<trains::types::Collision> {
    size_t operator()(const trains::types::Collision& collision) const {
        size_t h1 = std::hash<core::ecs::EntityID>()(collision.entityA);
        size_t h2 = std::hash<core::ecs::EntityID>()(collision.entityB);

        size_t h_min = std::min(h1, h2);
        size_t h_max = std::max(h1, h2);
        return h_min ^ (h_max << 1);
    }
};
} // namespace std

#endif // PROJECTS_TRAINS_TYPES_COLLISION_H
