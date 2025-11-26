#include "render_system.h"

#include <vector>

#include "core/attributes/camera.h"
#include "core/attributes/static_mesh.h"
#include "core/attributes/transform.h"
#include "core/attributes/light.h"
#include "core/render/drawable.h"
#include "core/render/renderer.h"
#include "core/ecs/archetype.h"
#include "core/ecs/types.h"
#include "core/managers/scene_manager.h"

namespace core::systems {

void RenderSystem::Start() {
	// Initialization if needed
}

void RenderSystem::StartAllArchetypes() {
	// Initialization per archetype if needed
}

void RenderSystem::Tick(float delta_time) {
	// Tick
}

void RenderSystem::TickAllArchetypes(float delta_time) {
	auto camera_archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<attributes::Camera,
										 attributes::Transform>());

	auto light_archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<attributes::Light,
										 attributes::Transform>());

	auto renderable_archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<attributes::StaticMesh,
										 attributes::Transform>());

	for (auto& camera_archetype_ref : camera_archetypes) {
		ecs::Archetype& camera_archetype = camera_archetype_ref.get();
		camera_archetype.ForEach([this, delta_time, &renderable_archetypes, &light_archetypes](ecs::EntityID camera_entity_id, size_t index) {
			attributes::Camera& camera = ecs_manager_.GetAttribute<attributes::Camera>(camera_entity_id);

			std::vector<render::Drawable> drawables;
			for (auto& archetype_ref : renderable_archetypes) {
				ecs::Archetype& archetype = archetype_ref.get();
				archetype.ForEach([this, delta_time, &archetype, &drawables, &camera](ecs::EntityID entity_id, size_t index) {
					attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
					attributes::StaticMesh& static_mesh = ecs_manager_.GetAttribute<attributes::StaticMesh>(entity_id);

					if ((static_mesh.culling_mask & camera.culling_mask) == 0) {
						return;
					}

					render::Drawable drawable;
					drawable.model_id = static_mesh.model_id;
					drawable.model_matrix = transform.GetModelMatrix();
					drawables.push_back(drawable);
				});
			}

			std::vector<render::DrawableLight> lights;
			for (auto& light_archetype_ref : light_archetypes) {
				ecs::Archetype& light_archetype = light_archetype_ref.get();
				light_archetype.ForEach([this, delta_time, &light_archetype, &lights, &camera](ecs::EntityID entity_id, size_t index) {
					attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
					attributes::Light& light_attr = ecs_manager_.GetAttribute<attributes::Light>(entity_id);

					if ((light_attr.culling_mask & camera.culling_mask) == 0) {
						return;
					}

					render::DrawableLight drawable_light;
					drawable_light.position = transform.position;
					drawable_light.color = light_attr.color;
					drawable_light.intensity = light_attr.intensity;
					drawable_light.linearAttenuation = light_attr.linear_attenuation;
					drawable_light.quadraticAttenuation = light_attr.quadratic_attenuation;
					lights.push_back(drawable_light);
				});
			}
			renderer_.Draw(drawables, lights, camera_entity_id);
		});
	}

	
}
} // namespace core::systems
