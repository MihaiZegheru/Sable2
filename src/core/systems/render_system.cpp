#include "render_system.h"

#include "core/attributes/static_mesh.h"
#include "core/attributes/transform.h"
#include "core/render/drawable.h"
#include "core/render/renderer.h"
#include "core/ecs/archetype.h"
#include "core/ecs/types.h"
#include "core/managers/scene_manager.h"

namespace core::systems {

void RenderSystem::Start() {
	// Initialization if needed
}

void RenderSystem::StartArchetype(ecs::Archetype& archetype) {
	// Initialization per archetype if needed
}

void RenderSystem::Tick(float delta_time) {
	// Tick
}

void RenderSystem::TickArchetype(ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time](ecs::EntityID entity_id, size_t index) {
		attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
		attributes::StaticMesh& static_mesh = ecs_manager_.GetAttribute<attributes::StaticMesh>(entity_id);

		render::Drawable drawable;
		drawable.model_id = static_mesh.model_id;
		drawable.model_matrix = transform.GetModelMatrix();

		render::Renderer& renderer = render::Renderer::GetInstance();

		// TODO: Remove hardcoded value

		renderer.Draw({drawable}, managers::SceneManager::GetInstance().GetMainCamera());
	});
}
} // namespace core::systems
