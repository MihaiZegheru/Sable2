#ifndef PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H
#define PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H

#include <vector>

#include <glm/vec3.hpp>

#include "core/ecs/types.h"

#include "projects/Trains/managers/map_manager.h"

namespace trains::attributes {

struct Train : public core::ecs::IAttribute {
	TileCoord current_tile_coord;
	TileCoord next_tile_coord;
	float speed;
	float acceleration;
	float deceleration;
	float max_speed;

	uint8_t selected_track_index;
	std::vector<TileCoord> track_selection_pool;
	bool just_spawned;

	bool is_locomotive;
	core::ecs::EntityID front_wagon;
	core::ecs::EntityID tail;
	float distance_to_front_wagon;
	bool checked_target;
	
	core::ecs::EntityID resource_entity;
	trains::types::ResourceType resource_type;
	glm::vec3 resource_offset;
	bool disconnected;

	Train() : current_tile_coord{0, 0}, 
			  next_tile_coord{0, 0},
			  speed{0.0f},
			  acceleration{4.0f},
			  deceleration{7.0f},
			  max_speed{30.0f},
			  selected_track_index{0},
			  track_selection_pool{},
			  is_locomotive{false},
			  distance_to_front_wagon{5.0f},
			  front_wagon{0},
			  just_spawned{true},
			  checked_target{false},
			  resource_entity{0},
			  resource_offset{0.0f, 3.0f, 1.0f},
			  disconnected{true} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H
