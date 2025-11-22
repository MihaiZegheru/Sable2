#ifndef PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H
#define PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H

#include "core/ecs/types.h"

#include "projects/Trains/managers/map_manager.h"

namespace trains::attributes {

struct Train : public core::ecs::IAttribute {
	TileCoord current_tile_coord;
	TileCoord next_tile_coord;
	float speed;

	bool moving;

	Train() : current_tile_coord{0, 0}, next_tile_coord{0, 0}, speed{0.0f}, moving{false} {}
};
} // namespace trains::attributes

#endif // PROJECTS_TRAINS_ATTRIBUTES_TRAIN_H
