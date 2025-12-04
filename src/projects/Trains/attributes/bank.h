#ifndef PROJECTS_TRAINS_ATTRIBUTES_BANK_H
#define PROJECTS_TRAINS_ATTRIBUTES_BANK_H

#include <vector>

#include <glm/vec3.hpp>
#include <unordered_set>

#include "core/ecs/types.h"
#include "projects/Trains/types/resource_type.h"
#include "projects/Trains/managers/map_manager.h"

namespace trains::attributes {

struct Bank : public core::ecs::IAttribute {
	std::vector<core::ecs::EntityID> display_points;
	glm::vec3 display_point_offsets[4] = {
		glm::vec3(-9.0f, 8.0f, 9.0f),
		glm::vec3(-5.0f, 6.0f, 10.0f),
		glm::vec3(5.0f, 6.0f, 10.0f),
		glm::vec3(9.0f, 8.0f, 9.0f)
	};

	trains::types::ResourceType resources_to_gather[4] = {
		trains::types::ResourceType::kDefault,
		trains::types::ResourceType::kDefault,
		trains::types::ResourceType::kDefault,
		trains::types::ResourceType::kDefault
	};

	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();

	Bank() : display_points{} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_BANK_H
