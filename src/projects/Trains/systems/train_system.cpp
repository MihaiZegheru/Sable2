#include "train_system.h"

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "core/managers/input_manager.h"

#include "projects/Trains/attributes/train.h"

#include <iostream>


namespace trains::systems {

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

		if (!train.is_locomotive) {
			return;
		}

		auto next_tiles_opt = map_manager_.GetNextTrackTiles(train.current_tile_coord);
		if (next_tiles_opt.has_value() && !next_tiles_opt->empty()) {
			train.track_selection_pool = *next_tiles_opt;
		} else {
			throw std::runtime_error("No available next track tiles from current position");
		}
	});
}

void TrainSystem::Tick(float delta_time) {
	// Tick
}

void TrainSystem::TickArchetype(core::ecs::Archetype& archetype, float delta_time) {
	archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
		trains::attributes::Train& train = ecs_manager_.GetAttribute<trains::attributes::Train>(entity_id);

		if (!train.is_locomotive) {
			core::ecs::EntityID front_wagon_id = train.front_wagon;
			core::attributes::Transform& front_wagon_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(front_wagon_id);
			core::attributes::Transform& transform = ecs_manager_.GetAttribute<core::attributes::Transform>(entity_id);
			trains::attributes::Train& front_wagon_train = ecs_manager_.GetAttribute<trains::attributes::Train>(front_wagon_id);

			float distance_to_front = glm::length(front_wagon_transform.position - transform.position);
			
			core::ecs::Entity& next_tile_entity = map_manager_.GetTileEntityAt(train.next_tile_coord);
			core::attributes::Transform& next_tile_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(next_tile_entity.id);

			glm::vec2 currentXZ(transform.position.x, transform.position.z);
			glm::vec2 targetXZ(next_tile_transform.position.x, next_tile_transform.position.z);

			glm::vec2 direction = targetXZ - currentXZ;
			float distance = glm::length(direction);

			if (distance < 0.01f) {
				transform.position.x = targetXZ.x;
				transform.position.z = targetXZ.y;
				train.checked_target = true;
				train.current_tile_coord = train.next_tile_coord;
				train.next_tile_coord = front_wagon_train.current_tile_coord;
		
				GeoPos next_direction = map_manager_.GetGeoPosBetween(train.current_tile_coord, train.next_tile_coord);
				transform.rotation.y = map_manager_.GetRotationByGeoPos(next_direction);
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
			if (train.checked_target && distance_to_front >= train.distance_to_front_wagon) {
				train.current_tile_coord = front_wagon_train.current_tile_coord;
				train.next_tile_coord = front_wagon_train.next_tile_coord;
				GeoPos next_direction = map_manager_.GetGeoPosBetween(train.current_tile_coord, train.next_tile_coord);
				transform.rotation.y = map_manager_.GetRotationByGeoPos(next_direction);
				
			}
			train.checked_target = false;

			return;
		}

		if (input_manager_.GetKey("D")) {
			TileCoord selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			core::ecs::EntityID selected_rail = map_manager_.GetRailEntityAt({train.next_tile_coord, selected_tile_coord});
			core::attributes::Transform& selected_track_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(selected_rail);

			selected_track_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
			train.selected_track_index++;
			train.selected_track_index %= train.track_selection_pool.size();
			if (train.track_selection_pool[train.selected_track_index] == train.current_tile_coord) {
				train.selected_track_index++;
				train.selected_track_index %= train.track_selection_pool.size();
			}

			TileCoord new_selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			// Dont select same tile as current if able
			if (new_selected_tile_coord == train.current_tile_coord && train.track_selection_pool.size() > 1) {
				train.selected_track_index++;
				train.selected_track_index %= train.track_selection_pool.size();
				new_selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			}
			core::ecs::EntityID new_selected_rail = map_manager_.GetRailEntityAt({train.next_tile_coord, new_selected_tile_coord});
			core::attributes::Transform& new_selected_track_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(new_selected_rail);
			new_selected_track_transform.scale = glm::vec3(13.0f, 13.0f, 13.0f);
		}

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
			train.next_tile_coord = train.track_selection_pool[train.selected_track_index];

			TileCoord selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			core::ecs::EntityID selected_rail = map_manager_.GetRailEntityAt({train.current_tile_coord, train.next_tile_coord});
			core::attributes::Transform& selected_track_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(selected_rail);
			selected_track_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);


			GeoPos next_direction = map_manager_.GetGeoPosBetween(train.current_tile_coord, train.next_tile_coord);
			transform.rotation.y = map_manager_.GetRotationByGeoPos(next_direction);


			auto next_tiles_opt = map_manager_.GetNextTrackTiles(train.next_tile_coord);
			if (next_tiles_opt.has_value() && !next_tiles_opt->empty()) {
				train.track_selection_pool = *next_tiles_opt;
			} else {
				throw std::runtime_error("No available next track tiles from current position");
			}

			// Set 0 as selected and resize
			train.selected_track_index = 0;
			TileCoord new_selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			// Dont select same tile as current if able
			if (new_selected_tile_coord == train.current_tile_coord && train.track_selection_pool.size() > 1) {
				train.selected_track_index = 1;
				new_selected_tile_coord = train.track_selection_pool[train.selected_track_index];
			}
			core::ecs::EntityID new_selected_rail = map_manager_.GetRailEntityAt({train.next_tile_coord, new_selected_tile_coord});
			core::attributes::Transform& new_selected_track_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(new_selected_rail);
			new_selected_track_transform.scale = glm::vec3(13.0f, 13.0f, 13.0f);

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
	});
}
} // namespace trains::systems