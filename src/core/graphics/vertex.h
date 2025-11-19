#ifndef CORE_GRAPHICS_VERTEX_H
#define CORE_GRAPHICS_VERTEX_H

#include <glm/glm.hpp>

namespace core::graphics {

struct Vertex {
    alignas (16) glm::vec4 position;
    alignas (16) glm::vec4 tangent;
    alignas (16) glm::vec4 normal;
    alignas (16) glm::vec2 texture_coords;
};
} // namespace core::graphics

#endif // CORE_GRAPHICS_VERTEX_H
