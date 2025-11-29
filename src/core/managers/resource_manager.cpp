#include "resource_manager.h"

#include "core/managers/shader_manager.h"
#include "core/render/render_model_data.h"
#include "core/graphics/texture.h"
#include "core/managers/asset_manager.h"

namespace core::managers {

namespace {

using namespace core::graphics;
using namespace core::render;

GLuint CreateGLTexture(const graphics::Texture& texture, GLenum internalFormat,
					   GLenum sizedInternalFormat) {
	GLuint gl_texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &gl_texture);

	glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gl_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gl_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTextureStorage2D(gl_texture, 3, sizedInternalFormat, texture.width, texture.height);
	glTextureSubImage2D(gl_texture, 0, 0, 0, texture.width, texture.height, internalFormat,
						GL_UNSIGNED_BYTE, texture.data);
	glGenerateTextureMipmap(gl_texture);

	return gl_texture;
}

GLuint LoadTexture(const graphics::Texture& texture){
	if (texture.gpu_uploaded) {
		return texture.gl_texture;
	}
	if (texture.channels == 3) {
		return CreateGLTexture(texture, GL_RGB, GL_RGBA8);
	}
	if (texture.channels == 4) {
		return CreateGLTexture(texture, GL_RGBA, GL_RGBA8);
	}
	return -1;
}

RenderMaterialData LoadRenderMaterialData(const graphics::Material& material) {
	RenderMaterialData material_data;
	material_data.diffuseTexture = -1;
	material_data.diffuseTexture = LoadTexture(material.diffuse_texture);
	material_data.normalMap = LoadTexture(material.normal_map);
	material_data.baseColor = material.base_color;
	material_data.textureMask = material.texture_mask;
	if (material.vertex_shader != -1 && material.fragment_shader != -1) {
		material_data.shader_program = managers::ShaderManager::GetInstance().CreateShaderProgram(
				material.vertex_shader,
				material.fragment_shader);
	}
	return material_data;
}

RenderMeshData LoadRenderMeshData(const graphics::Mesh& mesh) {
	RenderMeshData mesh_data;
	mesh_data.indicesSize = mesh.indices.size();
	
	glCreateVertexArrays(1, &mesh_data.vao);
	glCreateBuffers(1, &mesh_data.vbo);

	GLuint vboBindingPoint = 0;
	glNamedBufferStorage(mesh_data.vbo, sizeof(graphics::Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayVertexBuffer(mesh_data.vao, vboBindingPoint, mesh_data.vbo, 0, sizeof(graphics::Vertex));
	
	GLuint vboPositionIndex = 0;
	glEnableVertexArrayAttrib(mesh_data.vao, vboPositionIndex);
	glVertexArrayAttribFormat(mesh_data.vao, vboPositionIndex, 4, GL_FLOAT, false, 0);
	glVertexArrayAttribBinding(mesh_data.vao, vboPositionIndex, vboBindingPoint);

	GLuint vboTangentIndex = 1;
	glEnableVertexArrayAttrib(mesh_data.vao, vboTangentIndex);
	glVertexArrayAttribFormat(mesh_data.vao, vboTangentIndex, 4, GL_FLOAT, false, offsetof(graphics::Vertex, tangent));
	glVertexArrayAttribBinding(mesh_data.vao, vboTangentIndex, vboBindingPoint);

	GLuint vboNormalIndex = 2;
	glEnableVertexArrayAttrib(mesh_data.vao, vboNormalIndex);
	glVertexArrayAttribFormat(mesh_data.vao, vboNormalIndex, 4, GL_FLOAT, false, offsetof(graphics::Vertex, normal));
	glVertexArrayAttribBinding(mesh_data.vao, vboNormalIndex, vboBindingPoint);

	GLuint vboTextureIndex = 3;
	glEnableVertexArrayAttrib(mesh_data.vao, vboTextureIndex);
	glVertexArrayAttribFormat(mesh_data.vao, vboTextureIndex, 2, GL_FLOAT, false, offsetof(graphics::Vertex, texture_coords));
	glVertexArrayAttribBinding(mesh_data.vao, vboTextureIndex, vboBindingPoint);

	glCreateBuffers(1, &mesh_data.ebo);
	glNamedBufferStorage(mesh_data.ebo, sizeof(GLuint) * mesh.indices.size(), mesh.indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(mesh_data.vao, mesh_data.ebo);
	return mesh_data;
}

std::vector<RenderMaterialData> LoadRenderMaterialDatas(
		const std::vector<graphics::Material>& materials) {
	std::vector<RenderMaterialData> material_datas(materials.size());
	for (size_t i = 0; i < materials.size(); ++i) {
		material_datas[i] = LoadRenderMaterialData(materials[i]);
	}
	return material_datas;
}

std::vector<RenderMeshData> LoadRenderMeshDatas(const std::vector<graphics::Mesh>& meshes) {
	std::vector<RenderMeshData> mesh_datas(meshes.size());
	for (size_t i = 0; i < meshes.size(); ++i) {
		mesh_datas[i] = LoadRenderMeshData(meshes[i]);
	} 
	return mesh_datas;
}

RenderModelData LoadRenderModelData(const graphics::Model& model) {
	RenderModelData model_data;
	model_data.meshInstances = model.mesh_instances;
	model_data.meshDatas = LoadRenderMeshDatas(model.meshes);
	model_data.materialDatas = LoadRenderMaterialDatas(model.materials);
	return model_data;
}

} // namespace

void ResourceManager::UploadModel(ModelID model_id) {
	auto model_ref = asset_manager_.GetModel(model_id);
    model_id_to_render_data_[model_id] = LoadRenderModelData(model_ref.get());
}
} // namespace core::managers
