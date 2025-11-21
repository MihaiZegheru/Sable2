#include <iostream>

#include "core/ecs/ecs_manager.h"
#include "core/ecs/types.h"
#include "core/attributes/transform.h"
#include "core/attributes/camera.h"
#include "core/systems/camera_system.h"
#include "core/systems/render_system.h"
#include "core/render/renderer.h"
#include "core/platform/window.h"
#include "core/assetloader/asset_loader_manager.h"
#include "core/attributes/static_mesh.h"

#include "projects/Trains/managers/map_manager.h"

using namespace core;

int WINDOW_WIDTH = 2000;
int WINDOW_HEIGHT = 1200;

void OnWindowResize(GLFWwindow* window, int width, int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}


graphics::Mesh CreateSquare(float size, glm::vec3 origin) {
	graphics::Mesh squareMesh;
	float halfSize = size / 2.0f;
	glm::vec2 offset(0.0f, 0.0f);
	glm::vec3 adjustedOrigin = origin + glm::vec3(offset.x, offset.y, 0.0f);

	squareMesh.vertices = {
		{ glm::vec4(adjustedOrigin.x - halfSize, adjustedOrigin.y - halfSize, adjustedOrigin.z, 1.0f), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec2(0.0f, 0.0f) },
		{ glm::vec4(adjustedOrigin.x + halfSize, adjustedOrigin.y - halfSize, adjustedOrigin.z, 1.0f), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec2(1.0f, 0.0f) },
		{ glm::vec4(adjustedOrigin.x + halfSize, adjustedOrigin.y + halfSize, adjustedOrigin.z, 1.0f), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec2(1.0f, 1.0f) },
		{ glm::vec4(adjustedOrigin.x - halfSize, adjustedOrigin.y + halfSize, adjustedOrigin.z, 1.0f), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec2(0.0f, 1.0f) }
	};
	squareMesh.vertices_count = squareMesh.vertices.size();
	squareMesh.faces_count = 1;

	squareMesh.indices = {
		0, 1, 2,
		2, 3, 0
	};
	return squareMesh;
}

void RegisterAttributesAndSystems() {
	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();
	ecs_manager.RegisterAttribute<attributes::Transform>();
	ecs_manager.RegisterAttribute<attributes::Camera>();
	ecs_manager.RegisterAttribute<attributes::StaticMesh>();

	core::ecs::ArchetypeSignature camera_signature;
	camera_signature.set(0); // Transform
	camera_signature.set(1); // Camera
	ecs_manager.RegisterSystem<systems::CameraSystem>(camera_signature);

	core::ecs::ArchetypeSignature render_signature;
	render_signature.set(0); // Transform
	render_signature.set(2); // StaticMesh
	ecs_manager.RegisterSystem<systems::RenderSystem>(render_signature);
}

int main() {
	Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Game Engine");
	render::Renderer& renderer = render::Renderer::GetInstance();
	assetloader::AssetLoaderManager& asset_loader = assetloader::AssetLoaderManager::GetInstance();
	
    auto model_res = asset_loader.GetModelByPath("Tiles/Debug/debug_tile_empty/debug_tile_empty.obj");
	if (model_res.has_value()) {
		graphics::Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader.LoadModel(model);
		renderer.LoadModel(model);
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	RegisterAttributesAndSystems();
	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();

	core::ecs::Entity entity = ecs_manager.CreateEntity();
	attributes::Transform transform;
	transform.position = glm::vec3(0.0f, 700.0f, 0.0f);
	transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
	ecs_manager.AddAttribute<attributes::Transform>(entity.id, transform);
	attributes::Camera camera;
	ecs_manager.AddAttribute<attributes::Camera>(entity.id, camera);

	glfwSetFramebufferSizeCallback(window.GetInstance(), OnWindowResize);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2, 0.2, 0.2, 1);
 
	trains::managers::MapManager map_manager;
	map_manager.GenerateMap(10);

	float delta_time = 0.001f; // Simulate 1 second per tick
	while (!glfwWindowShouldClose(window.GetInstance())) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs_manager.UpdateSystems(delta_time);

        glfwSwapBuffers(window.GetInstance());
        glfwPollEvents();
	}
}