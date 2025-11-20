#ifndef CORE_RENDER_RENDER_MODEL_DATA_H
#define CORE_RENDER_RENDER_MODEL_DATA_H

#include <vector>

#include "render_mesh_data.h"
#include "render_material_data.h"
#include "core/graphics/model.h"

namespace core::render {

struct RenderModelData {
	std::vector <graphics::Model::MeshInstance> meshInstances;
	std::vector <RenderMeshData> meshDatas;
	std::vector <RenderMaterialData> materialDatas;
};
} // namespace core::render

#endif // CORE_RENDER_RENDER_MODEL_DATA_H
