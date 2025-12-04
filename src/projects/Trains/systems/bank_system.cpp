#include "bank_system.h"

#include "core/ecs/archetype.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "projects/Trains/attributes/bank.h"

#include <iostream>

namespace trains::systems {

void BankSystem::Start() {

}

void BankSystem::StartAllArchetypes() {
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::Bank,
										 core::attributes::Transform>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
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

				trains::types::ResourceType res_type = static_cast<trains::types::ResourceType>(rand() % 4);

				display_point_mesh.model_id = map_manager_.GetModelIdByResourceType(res_type);

				ecs_manager_.AddAttribute<core::attributes::StaticMesh>(display_point_entity.id, display_point_mesh);

				bank.resources_to_gather[i] = res_type;
				map_manager_.SetBankAskingForResourceType(res_type, true);
			}
		});
	}
}

void BankSystem::Tick(float delta_time) {

}

void BankSystem::TickAllArchetypes(float delta_time) {

	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::Bank>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
		archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
			trains::attributes::Bank& bank = ecs_manager_.GetAttribute<trains::attributes::Bank>(entity_id);
			std::cout << "Ticking Bank Entity ID: " << entity_id << std::endl;
			// Check any resource updates
			bool all_resources_gathered = true;
			for (size_t i = 0; i < 4; ++i) {
				if (bank.resources_to_gather[i] == ResourceType::kDefault) {
					continue;
				}
				all_resources_gathered = false;
				if (map_manager_.IsBankAskingForResourceType(bank.resources_to_gather[i])) {
					continue;
				}

				// Resource delivered, hide the display point
				core::attributes::StaticMesh& display_mesh = ecs_manager_.GetAttribute<core::attributes::StaticMesh>(bank.display_points[i]);
				display_mesh.is_visible = false;
				bank.resources_to_gather[i] = ResourceType::kDefault;
				std::cout << "Bank Entity ID: " << entity_id << " has received resource type: " << static_cast<int>(bank.resources_to_gather[i]) << std::endl;
			}
			std::cout << "All resources gathered: " << all_resources_gathered << std::endl;
			if (all_resources_gathered) {
				// Handle spawning new resources to gather
				for (size_t i = 0; i < 4; ++i) {
					core::ecs::EntityID display_point_entity_id = bank.display_points[i];
					core::attributes::StaticMesh& display_point_mesh = ecs_manager_.GetAttribute<core::attributes::StaticMesh>(display_point_entity_id);

					trains::types::ResourceType res_type = static_cast<trains::types::ResourceType>(rand() % 4);

					display_point_mesh.model_id = map_manager_.GetModelIdByResourceType(res_type);
					display_point_mesh.is_visible = true;

					bank.resources_to_gather[i] = res_type;
					map_manager_.SetBankAskingForResourceType(res_type, true);
				}
			}
		});
	}
}
} // namespace trains::systems
