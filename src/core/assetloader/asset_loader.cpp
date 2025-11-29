#include "asset_loader.h"

#include <algorithm>
#include <string>
#include <vector>
#include <string_view>
#include <cstdint>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <glad/glad.h>

#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/graphics/model.h"
#include "core/graphics/texture.h"

namespace core::assetloader {

namespace {

graphics::Model::MeshInstance LoadMeshInstance(const aiScene* const& aimpScene,
											   const aiNode* const& aimp_node, size_t mesh_index) {
	graphics::Model::MeshInstance mesh_instance;
	mesh_instance.mesh_index = mesh_index;
	mesh_instance.material_index = aimpScene->mMeshes[mesh_index]->mMaterialIndex;

	if (*aimp_node->mTransformation[mesh_index]) {
		mesh_instance.transformation_matrix = static_cast<glm::mat4>(*aimp_node->mTransformation[mesh_index]);
	} else {
		mesh_instance.transformation_matrix = glm::mat4(1);
	}
	return mesh_instance;
}

std::vector<graphics::Model::MeshInstance> LoadMeshInstances(const aiScene* const& aimpScene,
															 const aiNode* const& aimp_node) {
	std::vector<graphics::Model::MeshInstance> mesh_instances(aimp_node->mNumMeshes);
	for (unsigned int i = 0; i < aimp_node->mNumMeshes; ++i) {
		mesh_instances[i] = LoadMeshInstance(aimpScene, aimp_node, i);
	}
	for (size_t i = 0; i < aimp_node->mNumChildren; ++i) {
		std::vector<graphics::Model::MeshInstance> child_mesh_instances = LoadMeshInstances(aimpScene, aimp_node->mChildren[i]);
		mesh_instances.insert(mesh_instances.end(), child_mesh_instances.begin(), child_mesh_instances.end());
	}
	return mesh_instances;
}

graphics::Texture LoadTexture(const std::string& path) {
	graphics::Texture texture;
	int width, height, channels;
	texture.data = static_cast<uint8_t*>(stbi_load(path.c_str(), &width, &height, &channels, 0));
	texture.width = width;
	texture.height = height;
	texture.channels = channels;
	return texture;
}

graphics::Material LoadMaterial(aiMaterial* aimp_material, const std::string& path) { 
	graphics::Material material;

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
	return material;
}

graphics::Vertex LoadVertex(aiMesh* aimp_mesh, size_t index) {
	graphics::Vertex vertex;
	vertex.position = glm::vec4(aimp_mesh->mVertices[index].x, aimp_mesh->mVertices[index].y, aimp_mesh->mVertices[index].z, 1);
	vertex.normal = glm::vec4(aimp_mesh->mNormals[index].x, aimp_mesh->mNormals[index].y, aimp_mesh->mNormals[index].z, 1);
	vertex.tangent = glm::vec4(aimp_mesh->mTangents[index].x, aimp_mesh->mTangents[index].y, aimp_mesh->mTangents[index].z, 1);
	
	if (aimp_mesh->mTextureCoords[0]) {
		const aiVector3D &aimp_texture_coords = aimp_mesh->mTextureCoords[0][index];
		vertex.texture_coords = glm::vec2(aimp_texture_coords.x, aimp_texture_coords.y);
	}
	return vertex;
}

std::vector<unsigned int> LoadIndices(aiMesh* aimp_mesh) {
	std::vector<unsigned int> indices;
	for (size_t i = 0; i < aimp_mesh->mNumFaces; ++i) {
		size_t indices_count = aimp_mesh->mFaces[i].mNumIndices;
		for (size_t j = 0; j < indices_count; ++j) {
			indices.push_back(aimp_mesh->mFaces[i].mIndices[j]);
		}
	}
	return indices;
}

std::vector<graphics::Vertex> LoadVertices(aiMesh* aimp_mesh) {
	std::vector<graphics::Vertex> vertices;
	vertices.resize(aimp_mesh->mNumVertices);
	for (size_t i = 0; i < aimp_mesh->mNumVertices; ++i) {
		vertices[i] = LoadVertex(aimp_mesh, i);
	}
	return vertices;
}

graphics::Mesh LoadMesh(aiMesh* aimp_mesh) {
	graphics::Mesh mesh;
	mesh.vertices = LoadVertices(aimp_mesh);
	mesh.indices = LoadIndices(aimp_mesh);
	return mesh;
}

std::vector<graphics::Material> LoadMaterials(const aiScene* const& aimp_scene, const std::string& path) {
	std::vector<graphics::Material> materials;
	materials.resize(aimp_scene->mNumMaterials);
	for (size_t i = 0; i < aimp_scene->mNumMaterials; ++i) {
		materials[i] = LoadMaterial(aimp_scene->mMaterials[i], path);
	}
	return materials;
}

std::vector<graphics::Mesh> LoadMeshes(const aiScene* const& aimp_scene) {
	std::vector<graphics::Mesh> meshes(aimp_scene->mNumMeshes);
	for (size_t i = 0; i < aimp_scene->mNumMeshes; ++i) {
		meshes[i] = LoadMesh(aimp_scene->mMeshes[i]);
	}
	return meshes;
}

graphics::Model LoadModel(const aiScene* const& aimp_scene, const std::string& path) {
	graphics::Model model;
	model.meshes = LoadMeshes(aimp_scene);
	model.materials = LoadMaterials(aimp_scene, std::filesystem::path(path).parent_path().string());
	model.mesh_instances = LoadMeshInstances(aimp_scene, aimp_scene->mRootNode);
	return model;
}
} // namespace

graphics::Model ImportModel(const std::string& path) {
	Assimp::Importer importer;

    aiScene* aimp_scene = nullptr;
    aimp_scene = const_cast<aiScene*>(importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType));

	return LoadModel(aimp_scene, path);
}
} // namespace core::assetloader