#include <iostream>

#include "core/ecs/ecs_manager.h"
#include "core/ecs/types.h"
#include "core/attributes/transform.h"
#include "core/attributes/camera.h"
#include "core/attributes/light.h"
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
#include "core/render/renderer.h"
#include "core/render/drawablelight.h"
#include "core/graphics/mesh.h"
#include "core/graphics/model.h"
#include "core/graphics/texture.h"
#include "core/graphics/material.h"

#include "projects/Trains/managers/map_manager.h"
#include "projects/Trains/attributes/train.h"
#include "projects/Trains/systems/train_system.h"
#include "projects/Trains/attributes/bank.h"
#include "projects/Trains/attributes/resource_generator.h"
#include "projects/Trains/systems/bank_system.h"
#include "projects/Trains/systems/resource_system.h"
#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/systems/collision_system.h"

using namespace core;
using namespace trains;

const uint32_t kGameCullingLayerDefault = 0x00000001;
const uint32_t kGameCullingLayerMinimap = 0x00000002;
const uint32_t kGameCullingLayerUI = 0x00000004;

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
    { glm::vec4(adjustedOrigin.x - halfSize, adjustedOrigin.y - halfSize, adjustedOrigin.z, 1.0f),
      glm::vec4(1,0,0,0), glm::vec4(0,0,1,0), glm::vec2(0.0f, 0.0f) },

    { glm::vec4(adjustedOrigin.x + halfSize, adjustedOrigin.y - halfSize, adjustedOrigin.z, 1.0f),
      glm::vec4(1,0,0,0), glm::vec4(0,0,1,0), glm::vec2(1.0f, 0.0f) },

    { glm::vec4(adjustedOrigin.x + halfSize, adjustedOrigin.y + halfSize, adjustedOrigin.z, 1.0f),
      glm::vec4(1,0,0,0), glm::vec4(0,0,1,0), glm::vec2(1.0f, 1.0f) },

    { glm::vec4(adjustedOrigin.x - halfSize, adjustedOrigin.y + halfSize, adjustedOrigin.z, 1.0f),
      glm::vec4(1,0,0,0), glm::vec4(0,0,1,0), glm::vec2(0.0f, 1.0f) }
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
	ecs_manager.RegisterAttribute<trains::attributes::Bank>();
	ecs_manager.RegisterAttribute<trains::attributes::ResourceGenerator>();
	ecs_manager.RegisterAttribute<trains::attributes::BoxCollider>();
	ecs_manager.RegisterAttribute<core::attributes::Light>();

	ecs_manager.RegisterSystem<core::systems::CameraSystem,
							   core::attributes::Transform,
							   core::attributes::Camera>();

	ecs_manager.RegisterSystem<core::systems::RenderSystem,
							   core::attributes::Transform,
							   core::attributes::StaticMesh,
							   core::attributes::Camera,
							   core::attributes::Light>();

	ecs_manager.RegisterSystem<trains::systems::TrainSystem,
							   core::attributes::Transform,
							   core::attributes::StaticMesh,
							   trains::attributes::Train>();

	ecs_manager.RegisterSystem<core::systems::FollowSystem,
							   core::attributes::Transform,
							   core::attributes::Follow>();
							   
	ecs_manager.RegisterSystem<trains::systems::BankSystem,
							   core::attributes::Transform,
							   trains::attributes::Bank>();

	ecs_manager.RegisterSystem<trains::systems::ResourceSystem,
							   core::attributes::Transform,
							   trains::attributes::ResourceGenerator>();

	ecs_manager.RegisterSystem<trains::systems::CollisionSystem,
							   core::attributes::Transform,
							   trains::attributes::BoxCollider>();
}

int main() {
	Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Game Engine");

	RegisterAttributesAndSystems();

    core::managers::InputManager& inputManager = core::managers::InputManager::GetInstance();
	trains::managers::MapManager& map_manager = trains::managers::MapManager::GetInstance();
	core::managers::SceneManager& scene_manager = core::managers::SceneManager::GetInstance();
	trains::managers::CollisionManager& collision_manager = trains::managers::CollisionManager::GetInstance();

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
	core::managers::ShaderManager& shader_manager = core::managers::ShaderManager::GetInstance();

	glfwSetFramebufferSizeCallback(window.GetInstance(), OnWindowResize);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2, 0.2, 0.2, 1);


	map_manager.GenerateMap(10);
	TileCoord starting_tile_coords = map_manager.GetStartingTile();

	
	core::ecs::Entity train = ecs_manager.CreateEntity();
	core::attributes::Transform train_transform;
	train_transform.position = glm::vec3(0.0f, 7.0f, 0.0f);
	train_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(train.id, train_transform);
	core::attributes::StaticMesh train_mesh;
	train_mesh.model_id = model_id;
	train_mesh.culling_mask = kGameCullingLayerDefault;
	ecs_manager.AddAttribute<core::attributes::StaticMesh>(train.id, train_mesh);
	trains::attributes::Train train_attr;
	train_attr.current_tile_coord = starting_tile_coords;
	train_attr.next_tile_coord = starting_tile_coords;
	train_attr.speed = 15.0f;
	train_attr.is_locomotive = true;
	ecs_manager.AddAttribute<trains::attributes::Train>(train.id, train_attr);
	trains::attributes::BoxCollider train_collider;
	train_collider.size = glm::vec3(10.0f, 10.0f, 20.0f);
	ecs_manager.AddAttribute<trains::attributes::BoxCollider>(train.id, train_collider);

	core::ecs::Entity entity = ecs_manager.CreateEntity();
	core::attributes::Transform transform;
	transform.position = glm::vec3(0.0f, 700.0f, 0.0f);
	transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(entity.id, transform);
	core::attributes::Camera camera;
	camera.look_at = train.id;
	camera.width = WINDOW_WIDTH;
	camera.height = WINDOW_HEIGHT;
	camera.culling_mask = kGameCullingLayerDefault;
	camera.type = core::attributes::CameraType::kPerspective;
	ecs_manager.AddAttribute<core::attributes::Camera>(entity.id, camera);
	std::cout << "Created camera entity with ID: " << entity.id << std::endl;
	core::attributes::Follow follow;
	follow.target_entity = train.id;
	follow.offset = glm::vec3(0.0f, 60.0f, 100.0f);
	follow.match_rotation = true;
	ecs_manager.AddAttribute<core::attributes::Follow>(entity.id, follow);

	size_t num_lights = 5;
	std::vector<glm::vec3> light_positions = {
		glm::vec3(0.0f, 500.0f, 0.0f),
		glm::vec3(600.0f, 200.0f, 0.0f),
		glm::vec3(-600.0f, 200.0f, 0.0f),
		glm::vec3(0.0f, 200.0f, 600.0f),
		glm::vec3(0.0f, 200.0f, -600.0f)
	};
	for (int i = 0; i < num_lights; ++i) {
		core::ecs::Entity light_entity = ecs_manager.CreateEntity();
		core::attributes::Transform light_transform;
		light_transform.position = light_positions[i];
		ecs_manager.AddAttribute<core::attributes::Transform>(light_entity.id, light_transform);
		core::attributes::Light light_attr;
		light_attr.color = glm::vec3(1.0f, 1.0f, 0.95f);
		light_attr.intensity = 0.55f;
		light_attr.linear_attenuation = 0.000001f;
		light_attr.quadratic_attenuation = 0.000001f;
		ecs_manager.AddAttribute<core::attributes::Light>(light_entity.id, light_attr);
	}

	core::ecs::Entity minimap_camera = ecs_manager.CreateEntity();

	core::attributes::Transform transform2;
	transform2.position = glm::vec3(0.0f, 700.0f, 0.0f);
	transform2.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
	core::attributes::Camera camera2;
	camera2.width = 256;
	camera2.height = 256;
	camera2.culling_mask = kGameCullingLayerDefault;
	camera2.type = core::attributes::CameraType::kOrthographic;
	camera2.ortho_size = 10.0f;
	camera2.ortho_width = 256.0f;
	camera2.ortho_height = 256.0f;
	
	core::attributes::Follow follow2;
	follow2.target_entity = train.id;
	follow2.offset = glm::vec3(0.0f, 600.0f, 0.0f);

	GLuint fbo = 0, tex = 0, depth = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	const int TEX_WIDTH = 1024;

	// color texture
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	// attach
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	// depth renderbuffer
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

	// explicitly tell GL which color attachments we'll draw into
	GLenum drawbufs[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawbufs);

	// check completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Minimap FBO not complete: 0x" << std::hex << status << std::dec << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	camera2.fbo = fbo;
	camera2.color = tex;
	camera2.depth = depth;
	camera2.scope = core::attributes::CameraScope::kTexture;
	ecs_manager.AddAttribute<core::attributes::Camera>(minimap_camera.id, camera2);
	ecs_manager.AddAttribute<core::attributes::Transform>(minimap_camera.id, transform2);
	ecs_manager.AddAttribute<core::attributes::Follow>(minimap_camera.id, follow2);



	core::graphics::Mesh minimap_mesh = CreateSquare(256.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	core::graphics::Texture minimap_texture;
	minimap_texture.width = 256;
	minimap_texture.height = 256;
	minimap_texture.gpu_uploaded = true;
	minimap_texture.gl_texture = tex;
	core::graphics::Material minimap_material;
	minimap_material.base_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	minimap_material.texture_mask = 1 << 0;
	minimap_material.diffuse_texture = minimap_texture;
	minimap_material.vertex_shader = shader_manager.GetShaderFromPath("minimap/minimap.vert").value();
	minimap_material.fragment_shader = shader_manager.GetShaderFromPath("minimap/minimap.frag").value();
	core::graphics::Model minimap_model;
	minimap_model.id = 100;
	minimap_model.meshes.push_back(minimap_mesh);
	minimap_model.materials.push_back(minimap_material);
	minimap_model.mesh_instances.push_back({0, 0, glm::mat4(1.0f)});
	renderer_.LoadModel(minimap_model);

	core::ecs::Entity minimap_entity = ecs_manager.CreateEntity();
	core::attributes::Transform minimap_entity_transform;
	minimap_entity_transform.position = glm::vec3(256.0f, 256.0f, 0.0f);
	minimap_entity_transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(minimap_entity.id, minimap_entity_transform);
	core::attributes::StaticMesh minimap_entity_mesh;
	minimap_entity_mesh.model_id = minimap_model.id;
	minimap_entity_mesh.culling_mask = kGameCullingLayerUI;
	ecs_manager.AddAttribute<core::attributes::StaticMesh>(minimap_entity.id, minimap_entity_mesh);


	core::ecs::Entity ui_camera = ecs_manager.CreateEntity();
	core::attributes::Transform transform_ui;
	transform_ui.position = glm::vec3(0.0f, 0.0f, 100.0f);
	transform_ui.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	ecs_manager.AddAttribute<core::attributes::Transform>(ui_camera.id, transform_ui);
	core::attributes::Camera camera_ui;
	camera_ui.width = WINDOW_WIDTH;
	camera_ui.height = WINDOW_HEIGHT;
	camera_ui.scope = core::attributes::CameraScope::kUI;
	camera_ui.culling_mask = kGameCullingLayerUI;
	camera_ui.type = core::attributes::CameraType::kOrthographic;
	camera_ui.ortho_size = 1.0f;
	camera_ui.ortho_width = WINDOW_WIDTH;
	camera_ui.ortho_height = WINDOW_HEIGHT;
	ecs_manager.AddAttribute<core::attributes::Camera>(ui_camera.id, camera_ui);

	ecs_manager.StartSystems();
    Time::GetInstance().Init(glfwGetTime());
	while (!glfwWindowShouldClose(window.GetInstance())) {
        Time::GetInstance().ComputeDeltaTime(glfwGetTime());
		inputManager.Listen(window.GetInstance());
		collision_manager.ComputeCollisions();

		ecs_manager.UpdateSystems(Time::GetInstance().GetDeltaTime());

		// Clear collisions after processing
		collision_manager.ClearCollisions();

        glfwSwapBuffers(window.GetInstance());
        glfwPollEvents();
	}
}