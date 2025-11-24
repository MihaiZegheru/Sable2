#include "bank_system.h"

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "projects/Trains/attributes/bank.h"

#include <iostream>

namespace trains::systems {

void BankSystem::Start() {

}

void BankSystem::StartArchetype(core::ecs::Archetype& archetype) {
	std::cout << "BankSystem::StartArchetype called." << std::endl;
	archetype.ForEach([this, &archetype](core::ecs::EntityID entity_id, size_t index) {
		std::cout << "Initializing Bank Entity ID: " << entity_id << std::endl;
		trains::attributes::Bank& bank = ecs_manager_.GetAttribute<trains::attributes::Bank>(entity_id);
		for (size_t i = 0; i < 4; ++i) {
			core::ecs::Entity display_point_entity = ecs_manager_.CreateEntity();
			bank.display_points.push_back(display_point_entity.id);

			core::attributes::Transform display_point_transform;
			core::attributes::Transform& bank_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(entity_id);
			display_point_transform.position = bank_transform.position + bank.display_point_offsets[i];
			std::cout << "Display Point " << i << " Position: (" 
					  << bank_transform.position.x << ", "
					  << bank_transform.position.y << ", "
					  << bank_transform.position.z << ")" << std::endl;
			display_point_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
			ecs_manager_.AddAttribute<core::attributes::Transform>(display_point_entity.id, display_point_transform);
			core::attributes::StaticMesh display_point_mesh;
			display_point_mesh.model_id = 4;
			ecs_manager_.AddAttribute<core::attributes::StaticMesh>(display_point_entity.id, display_point_mesh);
		}
	});
}

void BankSystem::Tick(float delta_time) {

}

void BankSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::Bank& bank = ecs_manager_.GetAttribute<trains::attributes::Bank>(entity_id);

	});
} 
} // namespace trains::systems
