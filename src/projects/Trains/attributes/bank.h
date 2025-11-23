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
	std::unordered_set<trains::types::ResourceType> resources_to_gather;
	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();

	Bank() : display_points{},
			 resources_to_gather{} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_BANK_H
