#ifndef CORE_MANAGERS_RESOURCE_MANAGER_H
#define CORE_MANAGERS_RESOURCE_MANAGER_H

#include <cstddef>
#include <string>
#include <unordered_map>

#include "asset_manager.h"
#include "core/graphics/model.h"
#include "core/render/render_model_data.h"


namespace core::managers {

class ResourceManager {
public:
	static ResourceManager& GetInstance() {
		static ResourceManager instance;
		return instance;
	}

	// Uploads a model to OpenGL.
	void UploadModel(ModelID model_id);

	// Retrieves the RenderModelData for a given ModelID.
	render::RenderModelData& GetRenderModelData(ModelID model_id) {
		return model_id_to_render_data_.at(model_id);
	}

private:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

private:
	// Maps ModelIDs to their corresponding RenderModelData.
	std::unordered_map<ModelID, render::RenderModelData> model_id_to_render_data_;

	AssetManager& asset_manager_ = AssetManager::GetInstance();
};
} // namespace core::managers

#endif // CORE_MANAGERS_RESOURCE_MANAGER_H

