#include "asset_loader_manager.h"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/graphics/model.h"
#include "core/graphics/texture.h"

namespace core::assetloader {

using namespace core;

namespace {

	void GetMeshFromAssimp(aiMesh* aimp_mesh, graphics::Mesh& mesh) {
		mesh.vertices_count = aimp_mesh->mNumVertices;
		
		for (size_t i = 0; i < aimp_mesh->mNumVertices; ++i) {
			graphics::Vertex vertex;
			vertex.position = glm::vec4(aimp_mesh->mVertices[i].x, aimp_mesh->mVertices[i].y, aimp_mesh->mVertices[i].z, 1);
			vertex.normal = glm::vec4(aimp_mesh->mNormals[i].x, aimp_mesh->mNormals[i].y, aimp_mesh->mNormals[i].z, 1);
			vertex.tangent = glm::vec4(aimp_mesh->mTangents[i].x, aimp_mesh->mTangents[i].y, aimp_mesh->mTangents[i].z, 1);
			
			if (aimp_mesh->mTextureCoords[0]) {
				const aiVector3D &aimp_texture_coords = aimp_mesh->mTextureCoords[0][i];
				vertex.texture_coords = glm::vec2(aimp_texture_coords.x, aimp_texture_coords.y);
			}
			mesh.vertices.push_back(vertex);
		}
		mesh.faces_count = aimp_mesh->mNumFaces;
		for (size_t i = 0; i < aimp_mesh->mNumFaces; ++i) {
			size_t indices_count = aimp_mesh->mFaces[i].mNumIndices;
			for (size_t j = 0; j < indices_count; ++j) {
				mesh.indices.push_back(aimp_mesh->mFaces[i].mIndices[j]);
			}
		}
	}

	graphics::Texture LoadTexture(const std::string& path) {
		int width, height, channels;
		graphics::Texture texture;
		texture.data = static_cast<uint8_t*>(stbi_load(path.c_str(), &width, &height, &channels, 0));
		texture.width = width;
		texture.height = height;
		texture.channels = channels;
		return texture;
	}

	void GetMaterialFromAssimp(aiMaterial* aimp_material, graphics::Material& material, std::string& path) { 
		aiColor4D color;
		aiGetMaterialColor(aimp_material, AI_MATKEY_COLOR_DIFFUSE, &color);
		material.base_color = glm::vec4(color.r, color.g, color.b, color.a);

		int texture_mask = 0;
		aiString aimp_file_path;
		aimp_material->GetTexture(aiTextureType_DIFFUSE, 0, &aimp_file_path);
		std::string file_path = std::string(path + "/"
											+ std::string(aimp_file_path.C_Str()));
		graphics::Texture diffuse_texture = LoadTexture(file_path);
		material.diffuse_texture = diffuse_texture;
		if (diffuse_texture.data) {
			texture_mask |= 1 << 0;
		}

		aimp_material->GetTexture(aiTextureType_NORMALS, 0, &aimp_file_path);
		file_path = std::string(path + "/"
								+ std::string(aimp_file_path.C_Str()));
		graphics::Texture normal_map = LoadTexture(file_path);
		material.normal_map = normal_map;
		if (normal_map.data) {
			texture_mask |= 1 << 1;
		}
		material.texture_mask = texture_mask;
	}

	void PopulateModel(const aiScene* const& aimpScene, const aiNode* const& aimp_node,
					   graphics::Model& model) {
		for (unsigned int i = 0; i < aimp_node->mNumMeshes; ++i) {
			
			// TODO: Translate coords from localSpace to modelSpace

			graphics::Model::MeshInstance mesh_instance;
			mesh_instance.mesh_index = aimp_node->mMeshes[i];
			mesh_instance.material_index = aimpScene->mMeshes[mesh_instance.mesh_index]->mMaterialIndex;

			if (*aimp_node->mTransformation[i]) {
				mesh_instance.transformation_matrix = static_cast<glm::mat4>(*aimp_node->mTransformation[i]);
			} else {
				mesh_instance.transformation_matrix = glm::mat4(1);
			}
			model.mesh_instances.push_back(mesh_instance);
		}
		for (size_t i = 0; i < aimp_node->mNumChildren; ++i) {
			PopulateModel(aimpScene, aimp_node->mChildren[i], model);
		}
	}

	void GetModelFromAssimp(const aiScene* const& aimp_scene, graphics::Model& model, std::string& path) {
		model.meshes.resize(aimp_scene->mNumMeshes);
		for (size_t i = 0; i < aimp_scene->mNumMeshes; ++i) {
			GetMeshFromAssimp(aimp_scene->mMeshes[i], model.meshes[i]);
		}

		model.materials.resize(aimp_scene->mNumMaterials);
		for (size_t i = 0; i < aimp_scene->mNumMaterials; ++i) {
			GetMaterialFromAssimp(aimp_scene->mMaterials[i], model.materials[i], path);
		}

		const aiNode* const& aimpNode = aimp_scene->mRootNode;
		PopulateModel(aimp_scene, aimpNode, model);
	}

	// Checks if the file extension corresponds to a supported model format.
	bool IsModelExtension(const std::string& extension) {
		return extension == ".obj" || extension == ".fbx";
	}
} // namespace


AssetLoaderManager::AssetLoaderManager() {
	ParseResourcesFolder(std::string_view(ABSOLUTE_RESOURCE_DIR));
}

void AssetLoaderManager::ParseResourcesFolder(std::string_view folder_path) {
	auto dir_it = std::filesystem::directory_iterator(folder_path);

    for (const auto& entry : dir_it) {
        std::string file_path_str = entry.path().string();
        std::replace(file_path_str.begin(), file_path_str.end(), '\\', '/');
        auto filePath = std::filesystem::path(file_path_str);

        if (entry.is_directory()) {
            ParseResourcesFolder(file_path_str);
            continue;
        }

        std::string extension = filePath.extension().string();
        if (IsModelExtension(extension))
        {
            auto model = std::make_shared<graphics::Model>();
            model->id = GetNewID();

            id_to_model_[model->id] = model;
            id_to_path_[model->id] = file_path_str;
            path_to_model_[file_path_str] = model;
			id_to_dirrectory_[model->id] = filePath.parent_path().string();
        } else {
			// TODO: Handle other asset types
		}

    }
}

void AssetLoaderManager::LoadModel(graphics::Model& model) {
    Assimp::Importer importer;
    std::string path = id_to_path_[model.id];

    aiScene* aimp_scene = nullptr;
    aimp_scene = const_cast<aiScene*>(importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType));
    GetModelFromAssimp(aimp_scene, model, id_to_dirrectory_[model.id]);
}

std::optional<std::shared_ptr<graphics::Model>> AssetLoaderManager::GetModelByPath(const std::string& path) {
	const std::string absolute_file_path = std::string(ABSOLUTE_RESOURCE_DIR) + "/" + path;
	auto it = path_to_model_.find(absolute_file_path);
	if (it != path_to_model_.end()) {
		return it->second;
	}
	return std::nullopt;
}
} // namespace core::assetloader
