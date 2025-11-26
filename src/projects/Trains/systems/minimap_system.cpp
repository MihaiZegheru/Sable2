#include "minimap_system.h"

#include "projects/Trains/attributes/minimap_renderable.h"
#include "projects/Trains/managers/minimap_manager.h"

namespace trains::systems {

void MinimapSystem::Start() {

}
void MinimapSystem::StartAllArchetypes() {

}
void MinimapSystem::Tick(float delta_time) {

}

void MinimapSystem::TickAllArchetypes(float delta_time) {
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::MinimapRenderable>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
		archetype.ForEach([this, delta_time](core::ecs::EntityID entity_id, size_t index) {
			trains::attributes::MinimapRenderable& minimap_renderable = ecs_manager_.GetAttribute<trains::attributes::MinimapRenderable>(entity_id);
			core::ecs::EntityID camera_id = minimap_manager_.GetMinimapCameraEntityID();
			// Update minimap renderable logic here if needed
		});
	}
}
} // namespace trains::systems
