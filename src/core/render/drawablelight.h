#ifndef CORE_RENDER_DRAWABLELIGHT_H__
#define CORE_RENDER_DRAWABLELIGHT_H__

#include <glm/glm.hpp>

namespace core::render {

struct DrawableLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
    alignas(4) float linearAttenuation;
    alignas(4) float quadraticAttenuation;
};
}  // namespace core::render

#endif  // CORE_RENDER_DRAWABLELIGHT_H__