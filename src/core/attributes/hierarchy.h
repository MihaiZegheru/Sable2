#ifndef CORE_ATTRIBUTES_HIERARCHY_H
#define CORE_ATTRIBUTES_HIERARCHY_H

#include "core/ecs/types.h"

namespace core::attributes {

struct Hierarchy : ecs::IAttribute {
	ecs::EntityID parent_id;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_HIERARCHY_H
