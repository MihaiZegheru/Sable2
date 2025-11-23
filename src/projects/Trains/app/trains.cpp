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
#include "core/time/apptime.h"
#include "core/attributes/follow.h"
#include "core/systems/follow_system.h"
#include "core/managers/scene_manager.h"
#include "core/managers/input_manager.h"

#include "projects/Trains/managers/map_manager.h"
#include "projects/Trains/attributes/train.h"
#include "projects/Trains/systems/train_system.h"

using namespace core;
using namespace trains;

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
	ecs_manager.RegisterAttribute<core::attributes::Transform>();
	ecs_manager.RegisterAttribute<core::attributes::Camera>();
	ecs_manager.RegisterAttribute<core::attributes::StaticMesh>();
	ecs_manager.RegisterAttribute<trains::attributes::Train>();
	ecs_manager.RegisterAttribute<core::attributes::Follow>();

	core::ecs::ArchetypeSignature camera_signature;
	camera_signature.set(0); // Transform
	camera_signature.set(1); // Camera
	ecs_manager.RegisterSystem<core::systems::CameraSystem>(camera_signature);

	core::ecs::ArchetypeSignature render_signature;
	render_signature.set(0); // Transform
	render_signature.set(2); // StaticMesh
	ecs_manager.RegisterSystem<core::systems::RenderSystem>(render_signature);

	core::ecs::ArchetypeSignature train_signature;
	train_signature.set(0); // Transform
	train_signature.set(2); // StaticMesh
	train_signature.set(3); // Train
	ecs_manager.RegisterSystem<trains::systems::TrainSystem>(train_signature);

	core::ecs::ArchetypeSignature follow_signature;
	follow_signature.set(0); // Transform
	follow_signature.set(4); // Follow
	ecs_manager.RegisterSystem<core::systems::FollowSystem>(follow_signature);
}

int main() {
	Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Game Engine");

	RegisterAttributesAndSystems();

    core::managers::InputManager& inputManager = core::managers::InputManager::GetInstance();
	trains::managers::MapManager& map_manager = trains::managers::MapManager::GetInstance();
	core::managers::SceneManager& scene_manager = core::managers::SceneManager::GetInstance();

	core::assetloader::AssetLoaderManager& asset_loader_ = core::assetloader::AssetLoaderManager::GetInstance();
	core::render::Renderer& renderer_ = core::render::Renderer::GetInstance();
	auto model_res = asset_loader_.GetModelByPath("Train/train_locomotive/train_locomotive.obj");
	size_t model_id;
	if (model_res.has_value()) {
		graphics::Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		model_id = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	auto wagon_model_res = asset_loader_.GetModelByPath("Train/train_wagon/train_wagon.obj");
	size_t wagon_model_id;
	if (wagon_model_res.has_value()) {
		graphics::Model& wagon_model = *(wagon_model_res.value());
		std::cout << "Wagon model loaded with ID: " << wagon_model.id << std::endl;
		asset_loader_.LoadModel(wagon_model);
		renderer_.LoadModel(wagon_model);
		wagon_model_id = wagon_model.id;
	} else {
		std::cout << "Wagon model not found!" << std::endl;
	}

	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();

	glfwSetFramebufferSizeCallback(window.GetInstance(), OnWindowResize);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2, 0.2, 0.2, 1);


	map_manager.GenerateMap(6);
	TileCoord starting_tile_coords = map_manager.GetStartingTile();

	
	core::ecs::Entity train = ecs_manager.CreateEntity();
	core::attributes::Transform train_transform;
	train_transform.position = glm::vec3(0.0f, 7.0f, 0.0f);
	train_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(train.id, train_transform);
	core::attributes::StaticMesh train_mesh;
	train_mesh.model_id = model_id;
	ecs_manager.AddAttribute<core::attributes::StaticMesh>(train.id, train_mesh);
	trains::attributes::Train train_attr;
	train_attr.current_tile_coord = starting_tile_coords;
	train_attr.next_tile_coord = starting_tile_coords;
	train_attr.speed = 15.0f;
	train_attr.is_locomotive = true;
	ecs_manager.AddAttribute<trains::attributes::Train>(train.id, train_attr);

	float first_wagon_distance = 12.0f;
	float base_wagon_distance = 8.0f;

	core::ecs::EntityID wagon_id = train.id;
	for (int i = 0; i < 5; ++i) {
		core::ecs::Entity new_wagon = ecs_manager.CreateEntity();
		core::attributes::Transform new_wagon_transform;
		new_wagon_transform.position = glm::vec3(0.0f, 7.0f, -15.0f * (i + 2));
		new_wagon_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
		ecs_manager.AddAttribute<core::attributes::Transform>(new_wagon.id, new_wagon_transform);
		core::attributes::StaticMesh new_wagon_mesh;
		new_wagon_mesh.model_id = wagon_model_id;
		ecs_manager.AddAttribute<core::attributes::StaticMesh>(new_wagon.id, new_wagon_mesh);
		trains::attributes::Train new_wagon_attr;
		new_wagon_attr.current_tile_coord = starting_tile_coords;
		new_wagon_attr.next_tile_coord = starting_tile_coords;
		new_wagon_attr.speed = 15.0f;
		new_wagon_attr.front_wagon = wagon_id;
		if (i == 0) {
			new_wagon_attr.distance_to_front_wagon = first_wagon_distance;
		} else {
			new_wagon_attr.distance_to_front_wagon = base_wagon_distance;
		}
		ecs_manager.AddAttribute<trains::attributes::Train>(new_wagon.id, new_wagon_attr);

		wagon_id = new_wagon.id;
	}


	core::ecs::Entity entity = ecs_manager.CreateEntity();
	core::attributes::Transform transform;
	transform.position = glm::vec3(0.0f, 700.0f, 0.0f);
	transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(entity.id, transform);
	core::attributes::Camera camera;
	camera.look_at = train.id;
	ecs_manager.AddAttribute<core::attributes::Camera>(entity.id, camera);
	std::cout << "Created camera entity with ID: " << entity.id << std::endl;
	core::attributes::Follow follow;
	follow.target_entity = train.id;
	follow.offset = glm::vec3(0.0f, 50.0f, 80.0f);
	follow.match_rotation = true;
	ecs_manager.AddAttribute<core::attributes::Follow>(entity.id, follow);

	// follow locomotive
	scene_manager.SetMainCamera(entity.id);


	ecs_manager.StartSystems();
    Time::GetInstance().Init(glfwGetTime());
	while (!glfwWindowShouldClose(window.GetInstance())) {
        Time::GetInstance().ComputeDeltaTime(glfwGetTime());
		inputManager.Listen(window.GetInstance());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs_manager.UpdateSystems(Time::GetInstance().GetDeltaTime());

        glfwSwapBuffers(window.GetInstance());
        glfwPollEvents();
	}
}