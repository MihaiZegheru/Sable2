#include "resource_system.h"

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "projects/Trains/attributes/resource_generator.h"

namespace trains::systems {

void ResourceSystem::Start() {

}

void ResourceSystem::StartArchetype(core::ecs::Archetype& archetype) {

}

void ResourceSystem::Tick(float delta_time) {

}

void ResourceSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		// trains::attributes::ResourceGenerator& resource_generator = ecs_manager_.GetAttribute<trains::attributes::ResourceGenerator>(entity_id);

		// resource_generator.accumulated_time += delta_time;
		// if (resource_generator.accumulated_time >= resource_generator.generation_rate && !resource_generator.has_resource) {
		// 	resource_generator.has_resource = true;
		// 	resource_generator.accumulated_time = 0.0f;

		// 	core::attributes::StaticMesh& display_mesh = ecs_manager_.GetAttribute<core::attributes::StaticMesh>(resource_generator.display_point);
		// 	display_mesh.model_id = resource_generator.map_manager_.GetModelIdByResourceType(resource_generator.resource_type);
		// }
	});
}
} // namespace trains::systems