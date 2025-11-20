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
	
    auto model_res = asset_loader.GetModelByPath("Tiles/Grass_tile_NS/grass_tile_ns.obj");
	if (model_res.has_value()) {
		graphics::Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader.LoadModel(model);
		renderer.LoadModel(model);
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	auto cat_model_res = asset_loader.GetModelByPath("cat.fbx");
	if (cat_model_res.has_value()) {
		graphics::Model& cat_model = *(cat_model_res.value());
		std::cout << "Cat Model loaded with ID: " << cat_model.id << std::endl;
		asset_loader.LoadModel(cat_model);
		renderer.LoadModel(cat_model);
	} else {
		std::cout << "Cat Model not found!" << std::endl;
	}


	RegisterAttributesAndSystems();
	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();

	core::ecs::Entity entity = ecs_manager.CreateEntity();
	attributes::Transform transform;
	// transform.position = glm::vec3(0.0f, 0.0f, -50.0f);
	attributes::Camera camera;
	std::cout << "Created entity with ID: " << entity.id << std::endl;
	ecs_manager.AddAttribute<attributes::Transform>(entity.id, transform);
	std::cout << "Created entity with ID: " << entity.id << std::endl;
	ecs_manager.AddAttribute<attributes::Camera>(entity.id, camera);

	glfwSetFramebufferSizeCallback(window.GetInstance(), OnWindowResize);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2, 0.2, 0.2, 1);
    // glfwSetInputMode(window.GetInstance(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	graphics::Mesh squareMesh = CreateSquare(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	graphics::Material redMaterial;
	redMaterial.base_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	graphics::Model squareModel;
	squareModel.id = 0; // Assign an ID
	squareModel.materials.push_back(redMaterial);
	squareModel.meshes.push_back(squareMesh);
	squareModel.mesh_instances.push_back({0, 0, glm::mat4(1.0f)});
	renderer.LoadModel(squareModel);

	core::ecs::Entity entity2 = ecs_manager.CreateEntity();
	attributes::Transform transform2;
	transform2.position = glm::vec3(0.0f, 0.0f, -51.0f);
	transform2.scale = glm::vec3(100.0f, 100.0f, 100.0f);
	ecs_manager.AddAttribute<attributes::Transform>(entity2.id, transform2);
	auto square_trans = ecs_manager.GetAttribute<attributes::Transform>(entity2.id);
	std::cout << "Created entity with ID: " << entity2.id << " and assigned square model ID: " << squareModel.id << std::endl;
	std::cout << "Transform position: " << square_trans.position.x << ", " << square_trans.position.y << ", " << square_trans.position.z << std::endl;
	
	attributes::StaticMesh static_mesh2;
	static_mesh2.model_id = squareModel.id;
	ecs_manager.AddAttribute<attributes::StaticMesh>(entity2.id, static_mesh2);
	
	// auto square_trans = ecs_manager.GetAttribute<attributes::Transform>(entity2.id);
	// std::cout << "Created entity with ID: " << entity2.id << " and assigned square model ID: " << squareModel.id << std::endl;
	// std::cout << "Transform position: " << square_trans.position.x << ", " << square_trans.position.y << ", " << square_trans.position.z << std::endl;
	
	
	// core::ecs::Entity entity3 = ecs_manager.CreateEntity();
	// attributes::Transform transform3;
	// transform3.position = glm::vec3(2.0f, 0.0f, -50.0f);
	// transform3.scale = glm::vec3(10.0f, 10.0f, 10.0f);
	// ecs_manager.AddAttribute<attributes::Transform>(entity3.id, transform3);

	// attributes::StaticMesh static_mesh3;
	// static_mesh3.model_id = model_res.value()->id;
	// ecs_manager.AddAttribute<attributes::StaticMesh>(entity3.id, static_mesh3);

	float delta_time = 0.001f; // Simulate 1 second per tick
	while (!glfwWindowShouldClose(window.GetInstance())) {
		// glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs_manager.UpdateSystems(delta_time);

        glfwSwapBuffers(window.GetInstance());
        glfwPollEvents();
	}
}