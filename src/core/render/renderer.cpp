#include "renderer.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "drawable.h"
#include "core/graphics/model.h"
#include "core/graphics/texture.h"
#include "core/graphics/vertex.h"
#include "core/attributes/camera.h"
#include "core/attributes/transform.h"
#include "core/ecs/ecs_manager.h"


namespace core::render {

namespace {

	RenderMeshData LoadMesh(const graphics::Mesh& mesh)
	{
		RenderMeshData loadedData;

		glCreateVertexArrays(1, &loadedData.vao);
		glCreateBuffers(1, &loadedData.vbo);

		GLuint vboBindingPoint = 0;
		glNamedBufferStorage(loadedData.vbo, sizeof(graphics::Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(loadedData.vao, vboBindingPoint, loadedData.vbo, 0, sizeof(graphics::Vertex));

		GLuint vboPositionIndex = 0;
		glEnableVertexArrayAttrib(loadedData.vao, vboPositionIndex);
		glVertexArrayAttribFormat(loadedData.vao, vboPositionIndex, 4, GL_FLOAT, false, 0);
		glVertexArrayAttribBinding(loadedData.vao, vboPositionIndex, vboBindingPoint);

		GLuint vboTangentIndex = 1;
		glEnableVertexArrayAttrib(loadedData.vao, vboTangentIndex);
		glVertexArrayAttribFormat(loadedData.vao, vboTangentIndex, 4, GL_FLOAT, false, offsetof(graphics::Vertex, tangent));
		glVertexArrayAttribBinding(loadedData.vao, vboTangentIndex, vboBindingPoint);

		GLuint vboNormalIndex = 2;
		glEnableVertexArrayAttrib(loadedData.vao, vboNormalIndex);
		glVertexArrayAttribFormat(loadedData.vao, vboNormalIndex, 4, GL_FLOAT, false, offsetof(graphics::Vertex, normal));
		glVertexArrayAttribBinding(loadedData.vao, vboNormalIndex, vboBindingPoint);

		GLuint vboTextureIndex = 3;
		glEnableVertexArrayAttrib(loadedData.vao, vboTextureIndex);
		glVertexArrayAttribFormat(loadedData.vao, vboTextureIndex, 2, GL_FLOAT, false, offsetof(graphics::Vertex, texture_coords));
		glVertexArrayAttribBinding(loadedData.vao, vboTextureIndex, vboBindingPoint);

		glCreateBuffers(1, &loadedData.ebo);
		glNamedBufferStorage(loadedData.ebo, sizeof(GLuint) * mesh.indices.size(), mesh.indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(loadedData.vao, loadedData.ebo);

		return loadedData;
	}

	GLuint CreateGLTexture(const graphics::Texture& texture, GLenum internalFormat, GLenum sizedInternalFormat)
	{
		GLuint glTexture;
		glCreateTextures(GL_TEXTURE_2D, 1, &glTexture);

		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(glTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(glTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTextureStorage2D(glTexture, 3, sizedInternalFormat, texture.width, texture.height);
		glTextureSubImage2D(glTexture, 0, 0, 0, texture.width, texture.height, internalFormat, GL_UNSIGNED_BYTE, texture.data);
		glGenerateTextureMipmap(glTexture);

		return glTexture;
	}

	GLuint LoadTexture(const graphics::Texture& texture)
	{
		if (texture.channels == 3)
		{
			return CreateGLTexture(texture, GL_RGB, GL_RGBA8);
		}
		else if (texture.channels == 4)
		{
			return CreateGLTexture(texture, GL_RGBA, GL_RGBA8);
		}

		// Debug::LogError("Color component count not supported.");
		return -1;
	}

	RenderMaterialData LoadMaterial(const graphics::Material& material)
	{
		RenderMaterialData loadedMaterialData;
		loadedMaterialData.diffuseTexture = -1;
		loadedMaterialData.diffuseTexture = LoadTexture(material.diffuse_texture);
		loadedMaterialData.normalMap = LoadTexture(material.normal_map);
		loadedMaterialData.baseColor = material.base_color;
		loadedMaterialData.textureMask = material.texture_mask;
		return loadedMaterialData;
	}

	GLuint CreateDefaultShaderProgram(std::vector<char> fragmentShaderBuffer, std::vector<char> vertexShaderBuffer) {
		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		std::string vertexShaderSourceString = std::string(vertexShaderBuffer.begin(), vertexShaderBuffer.end());
		const GLchar* vertexShaderSource = vertexShaderSourceString.c_str();
		glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
		glCompileShader(vertexShader);

		int successStatus;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successStatus);
		char infoLog[512];
		if (!successStatus)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string fragmentShaderSourceString = std::string(fragmentShaderBuffer.begin(), fragmentShaderBuffer.end());
		const GLchar* fragmentShaderSource = fragmentShaderSourceString.c_str();
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successStatus);
		if (!successStatus)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successStatus);
		if (!successStatus)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << infoLog;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;
	}

	std::vector<char> ReadShader(std::string path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		size_t fileLen = static_cast<size_t>(file.tellg());

		std::vector<char> buffer(fileLen, 0);
		file.seekg(0);
		file.read(buffer.data(), fileLen);
		file.close();

		return buffer;
	}

	void InitGL()
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
	}
} // namespace

Renderer::Renderer() {
    InitShaders();
	InitGL();
}

void Renderer::InitShaders() {
	std::vector<char> fragmentShaderBuffer = ReadShader("../src/shaders/defaultfragmentshader.glsl");
	std::vector<char> vertexShaderBuffer = ReadShader("../src/shaders/defaultvertexshader.glsl");
	default_shader_program_ = CreateDefaultShaderProgram(fragmentShaderBuffer, vertexShaderBuffer);
	glUseProgram(default_shader_program_);
}

void Renderer::LoadModel(const graphics::Model& model)
{
    RenderModelData modelData;
    modelData.meshInstances = model.mesh_instances;
    for (const graphics::Mesh& mesh : model.meshes)
    {
        RenderMeshData meshData = LoadMesh(mesh);
        meshData.indicesSize = mesh.indices.size();
        modelData.meshDatas.push_back(meshData);
    }
    for (const graphics::Material& material : model.materials)
    {
        RenderMaterialData materialData = LoadMaterial(material);
        modelData.materialDatas.push_back(materialData);
    }
    id_to_render_data_[model.id] = modelData;
}

void Renderer::Draw(const std::vector <Drawable>& drawables, ecs::EntityID active_camera_id) {
	attributes::Camera& active_camera_attr = core::ecs::ECSManager::GetInstance().GetAttribute<attributes::Camera>(active_camera_id);
    glUniformMatrix4fv(1, 1, GL_FALSE, &active_camera_attr.view_matrix[0][0]);
    glUniformMatrix4fv(2, 1, GL_FALSE, &active_camera_attr.projection_matrix[0][0]);
	attributes::Transform& camera_transform = core::ecs::ECSManager::GetInstance().GetAttribute<attributes::Transform>(active_camera_id);
    glUniform3fv(4, 1, glm::value_ptr(camera_transform.position));

    for (const Drawable& drawable : drawables)
    {
        RenderModelData modelData = id_to_render_data_[drawable.model_id];
        const std::vector <graphics::Model::MeshInstance>& meshInstances = modelData.meshInstances;
        const std::vector <RenderMeshData>& meshDatas = modelData.meshDatas;
        const std::vector <RenderMaterialData>& materialDatas = modelData.materialDatas;

        for (const graphics::Model::MeshInstance& meshInstance : meshInstances)
        {
            glm::mat4 modelMatrix = meshInstance.transformation_matrix * drawable.model_matrix;
            glUniformMatrix4fv(0, 1, GL_FALSE, &modelMatrix[0][0]);
            const RenderMeshData& meshData = meshDatas[meshInstance.mesh_index];
            const RenderMaterialData& materialData = materialDatas[meshInstance.material_index];

            glUniform4fv(5, 1, glm::value_ptr(materialData.baseColor));
            glUniform1i(6, materialData.textureMask);

            glBindVertexArray(meshData.vao);
            glBindTextureUnit(0, materialData.diffuseTexture);
            glBindTextureUnit(1, materialData.normalMap);
            glDrawElements(GL_TRIANGLES, meshData.indicesSize, GL_UNSIGNED_INT, 0);
        }    
    }
}

} // namespace core::render