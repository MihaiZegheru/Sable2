#ifndef CORE_RENDER_DRAWABLE_H
#define CORE_RENDER_DRAWABLE_H

#include <glm/glm.hpp>

#include "core/assetloader/asset_loader_manager.h"

namespace core::render {

struct Drawable {
	glm::mat4 model_matrix;
	size_t model_id;
};
} // namespace core::render

#endif // CORE_RENDER_DRAWABLE_H
