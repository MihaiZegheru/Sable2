#ifndef CORE_ATTRIBUTES_STATIC_MESH_H
#define CORE_ATTRIBUTES_STATIC_MESH_H

#include <cstdint>

#include "core/ecs/types.h"

#include "renderable.h"

namespace core::attributes {

struct StaticMesh : IRenderable {
	uint16_t model_id;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_STATIC_MESH_H
