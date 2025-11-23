#ifndef PROJECTS_TRAINS_ATTRIBUTES_RESOURCE_GENERATOR_H
#define PROJECTS_TRAINS_ATTRIBUTES_RESOURCE_GENERATOR_H

#include "core/ecs/types.h"
#include "projects/Trains/types/resource_type.h"
#include "projects/Trains/managers/map_manager.h"
#include "core/ecs/entity.h"

namespace trains::attributes {

struct ResourceGenerator : public core::ecs::IAttribute {
	core::ecs::EntityID display_point;
	trains::types::ResourceType resource_type;
	float generation_rate;
	float accumulated_time;
	bool has_resource;
	trains::managers::MapManager& map_manager_ = trains::managers::MapManager::GetInstance();

	ResourceGenerator() : resource_type{trains::types::ResourceType::kFood},
						  generation_rate{5.0f},
						  accumulated_time{0.0f},
						  has_resource{false} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_RESOURCE_GENERATOR_H
