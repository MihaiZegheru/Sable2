#include "train_system.h"

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"

#include "projects/Trains/attributes/train.h"

#include <iostream>


namespace trains::systems {

namespace {

float GetRotationYForTrack(TrackType type) {
	switch (type) {
		case TrackType::kW_E:   return 0.0f;      // West → East
		case TrackType::kNW_E:  return 45.0f;     // NW → E
		case TrackType::kSW_E:  return -45.0f;    // SW → E
		case TrackType::kW_NE:  return 45.0f;     // W → NE
		case TrackType::kW_SE:  return -45.0f;    // W → SE
		case TrackType::kNE_SE: return 90.0f;     // NE → SE
		case TrackType::kNW_SW: return 135.0f;    // NW → SW
		// Add other cases as needed
		default: return 0.0f;                     // Default W-E
	}
}
} // namespace

void TrainSystem::Start() {
	
}

void TrainSystem::StartArchetype(core::ecs::Archetype& archetype) {
	archetype.ForEach([this, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::Train& train = ecs_manager_.GetAttribute<trains::attributes::Train>(entity_id);
		core::attributes::Transform& transform = ecs_manager_.GetAttribute<core::attributes::Transform>(entity_id);
		
		core::ecs::Entity& current_tile_entity = map_manager_.GetTileEntityAt(train.current_tile_coord);
		core::attributes::Transform& current_tile_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(current_tile_entity.id);

		transform.position.x = current_tile_transform.position.x;
		transform.position.z = current_tile_transform.position.z;	
	});
}

void TrainSystem::Tick(float delta_time) {
	// Tick
}

// TODO: Add start function to initialize train positions on the map

void TrainSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::Train& train = ecs_manager_.GetAttribute<trains::attributes::Train>(entity_id);
		core::attributes::Transform& transform = ecs_manager_.GetAttribute<core::attributes::Transform>(entity_id);

		core::ecs::Entity& next_tile_entity = map_manager_.GetTileEntityAt(train.next_tile_coord);
		core::attributes::Transform& next_tile_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(next_tile_entity.id);

		glm::vec2 currentXZ(transform.position.x, transform.position.z);
		glm::vec2 targetXZ(next_tile_transform.position.x, next_tile_transform.position.z);

		glm::vec2 direction = targetXZ - currentXZ;
		float distance = glm::length(direction);

		if (distance < 0.01f) {
			transform.position.x = targetXZ.x;
			transform.position.z = targetXZ.y;

			train.current_tile_coord = train.next_tile_coord;
			auto next_tiles_opt = map_manager_.GetNextTrackTiles(train.current_tile_coord);
			if (next_tiles_opt.has_value() && !next_tiles_opt->empty()) {
				train.next_tile_coord = next_tiles_opt->at(0); // Use first for now
			}
		} else {
			direction /= distance;
			glm::vec2 move = direction * train.speed * delta_time;
			
			// Overshoot correction
			if (glm::length(move) > distance) {
				move = direction * distance;
			}
			transform.position.x += move.x;
			transform.position.z += move.y;
		}

		// TrackType current_track_type = map_manager_.GetTrackTypeAt(train.current_tile_coord);
		// float targetRotationY = GetRotationYForTrack(current_track_type);

		// Smoothly interpolate rotation (optional) to avoid snapping
		// float rotationSpeed = 5.0f; // adjust speed
		// transform.rotation.y = glm::mix(transform.rotation.y, glm::radians(targetRotationY), rotationSpeed * delta_time);

	});
}
} // namespace trains::systems