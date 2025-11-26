#include "train_system.h"

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "core/managers/input_manager.h"
#include "core/assetloader/asset_loader_manager.h"
#include "core/graphics/model.h"

#include "projects/Trains/attributes/train.h"
#include "projects/Trains/attributes/resource_generator.h"
#include "projects/Trains/attributes/bank.h"
#include "projects/Trains/attributes/box_collider.h"
#include "projects/Trains/managers/map_manager.h"
#include "projects/Trains/managers/collision_manager.h"

#include <iostream>


namespace trains::systems {

namespace {
	const float first_wagon_distance = 12.0f;
	const float base_wagon_distance = 8.0f;
} // namespace

void TrainSystem::Start() {

}

void TrainSystem::StartAllArchetypes() {
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::Train,
										 core::attributes::Transform>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
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

			train.tail = entity_id;
			auto next_tiles_opt = map_manager_.GetNextTrackTiles(train.current_tile_coord);
			if (next_tiles_opt.has_value() && !next_tiles_opt->empty()) {
				train.track_selection_pool = *next_tiles_opt;
			} else {
				throw std::runtime_error("No available next track tiles from current position");
			}
		});
	}
}

void TrainSystem::Tick(float delta_time) {
	// Tick
}

void TrainSystem::TickAllArchetypes(float delta_time) {
	// std::cout << "Ticking Train Archetype with delta_time: " << delta_time << std::endl;
	auto archetypes = archetype_manager_.QueryArchetypes(
			ecs_manager_.GetSignatureFor<trains::attributes::Train,
										 core::attributes::Transform>());
	for (auto& archetype_ref : archetypes) {
		core::ecs::Archetype& archetype = archetype_ref.get();
		archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
			// std::cout << "Ticking Train Entity ID: " << entity_id << std::endl;
			trains::attributes::Train& train = ecs_manager_.GetAttribute<trains::attributes::Train>(entity_id);
			if (!train.is_locomotive) {
				if (train.disconnected) {
					return;
				}
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

				// not best but okay for now - to reduce speed when bumping into some other wagon
				train.speed = front_wagon_train.speed;

				if (!train.just_spawned) {
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
				}

				if ((train.just_spawned || train.checked_target) && distance_to_front >= train.distance_to_front_wagon) {
					train.current_tile_coord = front_wagon_train.current_tile_coord;
					train.next_tile_coord = front_wagon_train.next_tile_coord;
					GeoPos next_direction = map_manager_.GetGeoPosBetween(train.current_tile_coord, train.next_tile_coord);
					transform.rotation.y = map_manager_.GetRotationByGeoPos(next_direction);
					train.just_spawned = false;

				}
				train.checked_target = false;

				// Move resource entity with offset
				if (train.resource_entity != 0) {
					core::attributes::Transform& resource_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(train.resource_entity);
					resource_transform.position = transform.position + train.resource_offset;
					resource_transform.rotation = transform.rotation;
				}

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
			if (input_manager_.GetKey("space")) {
				// detahc tail wagon
				if (train.tail != entity_id) {
					trains::attributes::Train& tail_train = ecs_manager_.GetAttribute<trains::attributes::Train>(train.tail);
					train.tail = tail_train.front_wagon;
					tail_train.speed = 0.0f;
					tail_train.disconnected = true;
				}
			}

			// Handle acceleration and deceleration
			float move_input = input_manager_.GetAxis("vertical");
			if (move_input < 0) {
				train.speed -= train.deceleration * delta_time;
			} else if (move_input > 0) {
				train.speed += train.acceleration * delta_time;
			}
			train.speed = glm::clamp(train.speed, 0.0f, train.max_speed);

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


				// Handle resource pick-up
				if (auto resource_entity_opt = map_manager_.GetResourceEntityAt(train.current_tile_coord); resource_entity_opt.has_value()) {
					core::ecs::EntityID resource_entity_id = resource_entity_opt.value();
					trains::types::ResourceType resource_type = ecs_manager_.GetAttribute<trains::attributes::ResourceGenerator>(resource_entity_id).resource_type;
					std::cout << "Train picked up resource of type: " << static_cast<int>(resource_type) << " at tile ("
							<< train.current_tile_coord.q << ", " << train.current_tile_coord.r << ")" << std::endl;

					core::assetloader::AssetLoaderManager& asset_loader_ = core::assetloader::AssetLoaderManager::GetInstance();
					auto wagon_model_res = asset_loader_.GetModelByPath("Train/train_wagon/train_wagon.obj");
					size_t wagon_model_id;
					if (wagon_model_res.has_value()) {
						core::graphics::Model& wagon_model = *(wagon_model_res.value());
						wagon_model_id = wagon_model.id;
					} else {
						std::cout << "Wagon model not found!" << std::endl;
					}

					//Spawn new wagon at tail and update tail
					core::ecs::Entity new_wagon = ecs_manager_.CreateEntity();
					core::attributes::Transform new_wagon_transform;
					core::attributes::Transform& transform_tail = ecs_manager_.GetAttribute<core::attributes::Transform>(train.tail);
					new_wagon_transform.position = transform_tail.position;
					new_wagon_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
					ecs_manager_.AddAttribute<core::attributes::Transform>(new_wagon.id, new_wagon_transform);
					core::attributes::StaticMesh new_wagon_mesh;
					new_wagon_mesh.model_id = wagon_model_id;
					ecs_manager_.AddAttribute<core::attributes::StaticMesh>(new_wagon.id, new_wagon_mesh);
					trains::attributes::Train new_wagon_attr;
					trains::attributes::Train& train_tail = ecs_manager_.GetAttribute<trains::attributes::Train>(train.tail);
					new_wagon_attr.current_tile_coord = train_tail.current_tile_coord;
					new_wagon_attr.next_tile_coord = train_tail.current_tile_coord;
					new_wagon_attr.speed = train.speed;
					new_wagon_attr.just_spawned = true;
					new_wagon_attr.front_wagon = train.tail;
					new_wagon_attr.disconnected = false;
					if (train.tail == entity_id) {
						new_wagon_attr.distance_to_front_wagon = first_wagon_distance;
					} else {
						new_wagon_attr.distance_to_front_wagon = base_wagon_distance;
					}

					trains::attributes::BoxCollider new_wagon_collider;
					new_wagon_collider.size = glm::vec3(10.0f, 10.0f, 10.0f);
					ecs_manager_.AddAttribute<trains::attributes::BoxCollider>(new_wagon.id, new_wagon_collider);

					core::ecs::Entity resource_entity = ecs_manager_.CreateEntity();
					new_wagon_attr.resource_entity = resource_entity.id;
					core::attributes::Transform resource_transform;
					resource_transform.position = new_wagon_transform.position + train.resource_offset;
					resource_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
					ecs_manager_.AddAttribute<core::attributes::Transform>(resource_entity.id, resource_transform);
					core::attributes::StaticMesh resource_mesh;
					resource_mesh.model_id = map_manager_.GetModelIdByResourceType(resource_type);
					ecs_manager_.AddAttribute<core::attributes::StaticMesh>(resource_entity.id, resource_mesh);

					ecs_manager_.AddAttribute<trains::attributes::Train>(new_wagon.id, new_wagon_attr);
					std::cout << "New wagon spawned with ID: " << new_wagon.id << " connected to front wagon ID: " << train.tail << std::endl;

					train.tail = new_wagon.id;
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

			std::unordered_set<types::Collision, std::hash<types::Collision>> collisions;
			collision_manager_.GetCollisionsForEntity(entity_id, collisions);

			for (const auto& collision : collisions) {
				core::ecs::EntityID collided_entity_id = (collision.entityA == entity_id) ? collision.entityB : collision.entityA;
				trains::attributes::Train& collided_train = ecs_manager_.GetAttribute<trains::attributes::Train>(collided_entity_id);
				if (collided_train.disconnected) {
					if (collided_train.resource_entity != 0) {
						ecs_manager_.DestroyEntity(collided_train.resource_entity);
					}
					ecs_manager_.DestroyEntity(collided_entity_id);
					collision_manager_.RemoveCollidable(collided_entity_id);
					train.speed = 1.0f;
					std::cout << "Deleted disconnected wagon with ID: " << collided_entity_id << " due to collision." << std::endl;
				}
			}
		});
	}
}
} // namespace trains::systems