#ifndef CORE_ATTRIBUTES_RENDERABLE_H
#define CORE_ATTRIBUTES_RENDERABLE_H

#include <cstdint>

#include "core/ecs/types.h"

namespace core::attributes {

struct IRenderable : ecs::IAttribute {
	uint32_t culling_mask = 0xFFFFFFFF;
};
} // namespace core::attributes

#endif // CORE_ATTRIBUTES_RENDERABLE_H
