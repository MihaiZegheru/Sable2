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
#include "core/managers/shader_manager.h"
#include "core/managers/resource_manager.h"

#include <iostream>


namespace core::render {

GLuint PointLightBuffer = 0;
void* mappedLightBuffer = nullptr;

namespace {

void InitGL()
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

struct DrawCall {
    size_t model_id;
    uint32_t mesh_index;
    uint32_t material_index;
    glm::mat4 model_matrix; // final per-mesh model matrix
};
} // namespace

Renderer::Renderer() {
	InitGL();
	InitBuffers();
}

void Renderer::InitBuffers() {
    glCreateBuffers(1, &PointLightBuffer);
    glNamedBufferStorage(PointLightBuffer, sizeof(DrawableLight) * kMaxLightsCount, nullptr, GL_DYNAMIC_STORAGE_BIT);
    mappedLightBuffer = glMapNamedBuffer(PointLightBuffer, GL_WRITE_ONLY);
}

void Renderer::Draw(const std::vector<Drawable>& drawables,
                    const std::vector<DrawableLight>& lights,
                    const ecs::EntityID camera_id) {
    std::unordered_map<GLuint, std::vector<DrawCall>> grouped;
	grouped.reserve(16);
    for (const Drawable& drawable : drawables) {
        RenderModelData& modelData = resource_manager_.GetRenderModelData(drawable.model_id);
        const auto& meshInstances = modelData.meshInstances;

        for (const auto& meshInstance : meshInstances) {
            DrawCall dc;
            dc.model_id       = drawable.model_id;
            dc.mesh_index     = meshInstance.mesh_index;
            dc.material_index = meshInstance.material_index;
            dc.model_matrix   = meshInstance.transformation_matrix * drawable.model_matrix;

			const RenderMaterialData& mat = modelData.materialDatas[meshInstance.material_index];
			GLuint program = mat.shader_program.id;
			grouped[program].push_back(dc);
        }
    }

    attributes::Camera& active_camera_attr =
        ecs_manager_.GetAttribute<attributes::Camera>(camera_id);

    attributes::Transform& camera_transform =
        ecs_manager_.GetAttribute<attributes::Transform>(camera_id);

    glViewport(0, 0, active_camera_attr.width, active_camera_attr.height);

    if (lights.size() > kMaxLightsCount) {
        std::cerr << "Warning: Exceeding maximum number of lights. "
                     "Some lights will be ignored." << std::endl;
    }

    int truncatedLightsSize = std::min((int)lights.size(), (int)kMaxLightsCount);
    if (truncatedLightsSize > 0) {
        glNamedBufferSubData(core::render::PointLightBuffer,
                             0,
                             sizeof(core::render::DrawableLight) * truncatedLightsSize,
                             lights.data());
    }

	for (auto& bucket : grouped) {
		GLuint program = bucket.first;
		const std::vector<DrawCall>& calls = bucket.second;

		glUseProgram(program);

		// Camera
		glUniformMatrix4fv(1, 1, GL_FALSE, &active_camera_attr.view_matrix[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &active_camera_attr.projection_matrix[0][0]);
		glUniform3fv(4, 1, glm::value_ptr(camera_transform.position));

		// Lights
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, core::render::PointLightBuffer);
		glUniform1i(7, truncatedLightsSize);

		for (const DrawCall& dc : calls) {
			RenderModelData& modelData = resource_manager_.GetRenderModelData(dc.model_id);

			const RenderMeshData& meshData =
				modelData.meshDatas[dc.mesh_index];
			const RenderMaterialData& materialData =
				modelData.materialDatas[dc.material_index];

			// per-object
			glUniformMatrix4fv(0, 1, GL_FALSE, &dc.model_matrix[0][0]);

			// material
			glUniform4fv(5, 1, glm::value_ptr(materialData.baseColor));
			glUniform1i(6, materialData.textureMask);

			// mesh VAO
			glBindVertexArray(meshData.vao);

			// textures
			glBindTextureUnit(0, materialData.diffuseTexture);
			glBindTextureUnit(1, materialData.normalMap);

			glDrawElements(GL_TRIANGLES, meshData.indicesSize, GL_UNSIGNED_INT, 0);
		}
	}
}

} // namespace core::render