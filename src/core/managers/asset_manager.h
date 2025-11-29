#ifndef CORE_MANAGERS_ASSET_MANAGER_H
#define CORE_MANAGERS_ASSET_MANAGER_H

#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

#include "core/graphics/model.h"

namespace core::managers {

// Error messages
inline constexpr std::string_view kPathNotFound = "Path not found";

// Resources folder path
inline constexpr std::string_view kResourcesFolder = ABSOLUTE_RESOURCE_DIR;

using ModelID = size_t;

class AssetManager {
public:
    static AssetManager& GetInstance() {
		static AssetManager instance;
		return instance;
	}

	// Loads a model from the given path. Returns the ModelID on success or an error message on
	// failure.
	std::expected<ModelID, std::string_view> LoadModel(const std::string path);

	// Loads a model and returns its ModelID.
	inline ModelID LoadModel(const core::graphics::Model& model) {
		ModelID model_id = NextID();
		auto new_model = std::make_unique<core::graphics::Model>(model);
		new_model->id = model_id;
		id_to_model_[model_id] = std::move(new_model);
		return model_id;
	}

	// Retrieves a model by its ModelID.
	inline std::reference_wrapper<const core::graphics::Model> GetModel(ModelID model_id) {
		return *id_to_model_.at(model_id);
	}

private:
	AssetManager();
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	// Recursively parses the resources folder to index available assets.
	void ParseResourcesFolder(std::string_view folder_path);

	// Generates a new unique ID for a Model.
	// 0 is reserved.
	ModelID NextID() {
		return ++next_id_;
	}

private:
	// Maps file paths to ModelIDs. Used for user resource lookup.
	std::unordered_map<std::string, ModelID> path_to_model_id_;
	// Maps ModelIDs to Model instances.
	std::unordered_map<ModelID, std::unique_ptr<core::graphics::Model>> id_to_model_;

	// Used for assigning unique IDs to models.
	ModelID next_id_ = 0;
};
} // namespace core::managers

#endif // CORE_MANAGERS_ASSET_MANAGER_H
