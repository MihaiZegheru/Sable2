#include <iostream>

#include "core/ecs/ecs_manager.h"
#include "core/ecs/types.h"
#include "core/attributes/transform.h"
#include "core/attributes/camera.h"
#include "core/systems/camera_system.h"
#include "core/render/renderer.h"
#include "core/platform/window.h"

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

int main() {
	Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Game Engine");
	render::Renderer& renderer = render::Renderer::GetInstance();

	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();
	ecs_manager.RegisterAttribute<attributes::Transform>();
	ecs_manager.RegisterAttribute<attributes::Camera>();

	core::ecs::ArchetypeSignature camera_signature;
	camera_signature.set(0); // Transform
	camera_signature.set(1); // Camera
	ecs_manager.RegisterSystem<systems::CameraSystem>(camera_signature);

	core::ecs::Entity entity = ecs_manager.CreateEntity();
	attributes::Transform transform;
	attributes::Camera camera;
	ecs_manager.AddAttribute<attributes::Transform>(entity.id, transform);
	ecs_manager.AddAttribute<attributes::Camera>(entity.id, camera);

	core::ecs::Entity entity2 = ecs_manager.CreateEntity();
	attributes::Transform transform2;
	transform2.position = glm::vec3(0.0f, 0.0f, -2.0f);
	ecs_manager.AddAttribute<attributes::Transform>(entity2.id, transform2);

	glfwSetFramebufferSizeCallback(window.GetInstance(), OnWindowResize);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2, 0.2, 0.2, 1);
    // glfwSetInputMode(window.GetInstance(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	graphics::Mesh squareMesh = CreateSquare(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	graphics::Material redMaterial;
	redMaterial.base_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	graphics::Model squareModel;
	squareModel.id = 1; // Assign an ID
	squareModel.materials.push_back(redMaterial);
	squareModel.meshes.push_back(squareMesh);
	squareModel.mesh_instances.push_back({0, 0, glm::mat4(1.0f)});
	renderer.LoadModel(squareModel);

	render::Drawable squareDrawable;
	squareDrawable.model_id = squareModel.id;
	squareDrawable.model_matrix = transform2.GetModelMatrix();

	float delta_time = 0.001f; // Simulate 1 second per tick
	while (!glfwWindowShouldClose(window.GetInstance())) {
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs_manager.UpdateSystems(delta_time);
		renderer.Draw({squareDrawable}, entity);
        glfwSwapBuffers(window.GetInstance());
        glfwPollEvents();
	}
}