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

#include <iostream>

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

	std::vector<ecs::EntityID> scene_cameras;
	std::vector<ecs::EntityID> ui_cameras;
	std::vector<ecs::EntityID> texture_cameras;
	for (auto& camera_archetype_ref : camera_archetypes) {
		ecs::Archetype& camera_archetype = camera_archetype_ref.get();
		camera_archetype.ForEach([this, &scene_cameras, &ui_cameras, &texture_cameras](ecs::EntityID camera_entity_id, size_t index) {
			attributes::Camera& camera = ecs_manager_.GetAttribute<attributes::Camera>(camera_entity_id);
			switch (camera.scope) {
				case attributes::CameraScope::kScene:
					scene_cameras.push_back(camera_entity_id);
					break;
				case attributes::CameraScope::kUI:
					ui_cameras.push_back(camera_entity_id);
					break;
				case attributes::CameraScope::kTexture:
					texture_cameras.push_back(camera_entity_id);
					break;
				default:
					break;
			}
		});
	}

	// Pair of drawable and mask
	std::vector<std::pair<render::Drawable, uint32_t>> drawables;
	std::vector<std::pair<render::DrawableLight, uint32_t>> lights;
	for (auto& archetype_ref : renderable_archetypes) {
		ecs::Archetype& archetype = archetype_ref.get();
		archetype.ForEach([this, delta_time, &archetype, &drawables](ecs::EntityID entity_id, size_t index) {
			attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
			attributes::StaticMesh& static_mesh = ecs_manager_.GetAttribute<attributes::StaticMesh>(entity_id);
			if (!static_mesh.is_visible) {
				return;
			}

			render::Drawable drawable;
			drawable.model_id = static_mesh.model_id;
			drawable.model_matrix = transform.GetModelMatrix();
			drawables.push_back({drawable, static_mesh.culling_mask});
		});
	}
	for (auto& light_archetype_ref : light_archetypes) {
		ecs::Archetype& light_archetype = light_archetype_ref.get();
		light_archetype.ForEach([this, delta_time, &light_archetype, &lights](ecs::EntityID entity_id, size_t index) {
			attributes::Transform& transform = ecs_manager_.GetAttribute<attributes::Transform>(entity_id);
			attributes::Light& light_attr = ecs_manager_.GetAttribute<attributes::Light>(entity_id);

			render::DrawableLight drawable_light;
			drawable_light.position = transform.position;
			drawable_light.color = light_attr.color;
			drawable_light.intensity = light_attr.intensity;
			drawable_light.linearAttenuation = light_attr.linear_attenuation;
			drawable_light.quadraticAttenuation = light_attr.quadratic_attenuation;
			lights.push_back({drawable_light, light_attr.culling_mask});
		});
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Textures Pass
	for (ecs::EntityID texture_camera_id : texture_cameras) {
		std::vector<render::Drawable> pass_drawables;
		std::vector<render::DrawableLight> pass_lights;

		attributes::Camera& camera_attr = ecs_manager_.GetAttribute<attributes::Camera>(texture_camera_id);
		uint32_t culling_mask = camera_attr.culling_mask;

		for (const auto& [drawable, mask] : drawables) {
			if (mask & culling_mask) {
				pass_drawables.push_back(drawable);
			}
		}
		for (const auto& [light, mask] : lights) {
			if (mask & culling_mask) {
				pass_lights.push_back(light);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, camera_attr.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer_.Draw(pass_drawables, pass_lights, texture_camera_id);
		
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Scene Pass
	for (ecs::EntityID scene_camera_id : scene_cameras) {
		std::vector<render::Drawable> pass_drawables;
		std::vector<render::DrawableLight> pass_lights;

		attributes::Camera& camera_attr = ecs_manager_.GetAttribute<attributes::Camera>(scene_camera_id);
		uint32_t culling_mask = camera_attr.culling_mask;

		for (const auto& [drawable, mask] : drawables) {
			if (mask & culling_mask) {
				pass_drawables.push_back(drawable);
			}
		}
		for (const auto& [light, mask] : lights) {
			if (mask & culling_mask) {
				pass_lights.push_back(light);
			}
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, camera_attr.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer_.Draw(pass_drawables, pass_lights, scene_camera_id);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// UI Pass
	for (ecs::EntityID ui_camera_id : ui_cameras) {
		std::vector<render::Drawable> pass_drawables;
		std::vector<render::DrawableLight> pass_lights;

		attributes::Camera& camera_attr = ecs_manager_.GetAttribute<attributes::Camera>(ui_camera_id);
		uint32_t culling_mask = camera_attr.culling_mask;

		for (const auto& [drawable, mask] : drawables) {
			if (mask & culling_mask) {
				pass_drawables.push_back(drawable);
			}
		}
		for (const auto& [light, mask] : lights) {
			if (mask & culling_mask) {
				pass_lights.push_back(light);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, camera_attr.fbo);

		renderer_.Draw(pass_drawables, pass_lights, ui_camera_id);
	}

	glEnable(GL_DEPTH_TEST);
}
} // namespace core::systems
