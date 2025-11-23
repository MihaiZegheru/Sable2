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

	uint8_t selected_track_index;
	std::vector<TileCoord> track_selection_pool;
	bool just_spawned;

	bool is_locomotive;
	core::ecs::EntityID front_wagon;
	core::ecs::EntityID tail;
	float distance_to_front_wagon;
	bool checked_target;

	Train() : current_tile_coord{0, 0}, 
			  next_tile_coord{0, 0},
			  speed{0.0f},
			  selected_track_index{0},
			  track_selection_pool{},
			  is_locomotive{false},
			  distance_to_front_wagon{5.0f},
			  front_wagon{0},
			  just_spawned{true},
			  checked_target{false} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H
