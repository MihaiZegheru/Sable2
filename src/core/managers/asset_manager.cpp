#include "asset_manager.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include "core/graphics/model.h"
#include "core/assetloader/asset_loader.h"

#include <iostream>

namespace core::managers {

namespace {

// Checks if the file extension corresponds to a supported model format.
bool IsModelExtension(const std::string& extension) {
	return extension == ".obj" || extension == ".fbx";
}
} // namespace

AssetManager::AssetManager() {
	ParseResourcesFolder(kResourcesFolder);
}

void AssetManager::ParseResourcesFolder(std::string_view folder_path) {
	auto dir_it = std::filesystem::directory_iterator(folder_path);

    for (const auto& entry : dir_it) {
        std::string file_path_str = entry.path().string();
        std::replace(file_path_str.begin(), file_path_str.end(), '\\', '/');
        auto file_path = std::filesystem::path(file_path_str);

        if (entry.is_directory()) {
            ParseResourcesFolder(file_path_str);
            continue;
        }

        std::string extension = file_path.extension().string();
        if (IsModelExtension(extension)) {
			std::filesystem::path relative = file_path.lexically_relative(kResourcesFolder);
			std::string relative_str = relative.string();
			std::replace(relative_str.begin(), relative_str.end(), '\\', '/');
			path_to_model_id_[relative_str] = NextID();
        }
    }
}

std::expected<ModelID, std::string_view> AssetManager::LoadModel(const std::string path) {
	auto it = path_to_model_id_.find(path);
	if (it != path_to_model_id_.end()) {
		auto model = std::make_unique<graphics::Model>(assetloader::ImportModel(
				std::string(kResourcesFolder) + std::string("/") + path));

		ModelID model_id = it->second;
		model->id = model_id;
		id_to_model_[model_id] = std::move(model);
		
		return model_id;
	} else {
		return std::unexpected(kPathNotFound);
	}
}
} // namespace core::managers