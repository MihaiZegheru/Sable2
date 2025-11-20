#ifndef CORE_ASSETLOADER_ASSET_LOADER_MANAGER_H
#define CORE_ASSETLOADER_ASSET_LOADER_MANAGER_H

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/graphics/model.h"

namespace core::assetloader {

using namespace core;

using ModelID = size_t;

class AssetLoaderManager {
public:
	static AssetLoaderManager& GetInstance() {
		static AssetLoaderManager instance;
		return instance;
	}
	
	// Loads a model.
	void LoadModel(graphics::Model& model);
	// Retrieves a model by its path. Returns nullopt if not found.
	std::optional<std::shared_ptr<graphics::Model>> GetModelByPath(const std::string& path);

private:
	explicit AssetLoaderManager();
	// Recursively parses the resources folder to index available assets.
	void ParseResourcesFolder(std::string_view folder_path);

	// TODO: Migrate this logic to a Model Manager class.

	// Generates a new unique ID for a Model.
	ModelID GetNewID() { return next_id_++; } 

private:
	std::unordered_map<ModelID, std::string> id_to_path_;
	std::unordered_map<ModelID, std::shared_ptr<graphics::Model>> id_to_model_;
	std::unordered_map<std::string, std::shared_ptr<graphics::Model>> path_to_model_;
	std::unordered_map<ModelID, std::string> id_to_dirrectory_;

	ModelID next_id_ = 0;
};
} // namespace core::assetloader

#endif // CORE_ASSETLOADER_ASSET_LOADER_MANAGER_H