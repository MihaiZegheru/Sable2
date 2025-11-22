#include "map_manager.h"

#include <iostream>
#include <cstdlib>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <chrono>

#include "core/ecs/ecs_manager.h"
#include "core/attributes/transform.h"
#include "core/attributes/static_mesh.h"
#include "core/assetloader/asset_loader_manager.h"
#include "core/render/renderer.h"
#include "core/graphics/model.h"

using namespace core;
using namespace core::ecs;
using namespace core::attributes;
using namespace core::assetloader;
using namespace core::render;
using namespace core::graphics;

namespace trains::managers {

namespace {

const TileCoord neighbors[6] = {
	{1, 0}, {1, -1}, {0, -1},
	{-1, 0}, {-1, 1}, {0, 1}
};

TileCoord RandomEdgeTileFrom(int side, int radius) {
    int q, r;
    switch (side) {
        case 0: // right
            r = std::rand() % (0 - (-radius) + 1) + (-radius);
            q = radius;
            break;
        case 1: // top-right
            q = std::rand() % (radius - 0 + 1) + 0;
            r = -radius;
            break;
        case 2: // top-left
            q = std::rand() % (0 - (-radius) + 1) + (-radius);
            r = -radius - q;
            break;
        case 3: // left
            r = std::rand() % (radius - 0 + 1) + 0;
            q = -radius;
            break;
        case 4: // bottom-left
            q = std::rand() % (0 - (-radius) + 1) + (-radius);
            r = radius;
            break;
        case 5: // bottom-right
            q = std::rand() % (radius - 0 + 1) + 0;
            r = radius - q;
            break;
        default:
            q = r = 0;
            break;
    }
    return { q, r };
}

std::pair<TileCoord, TileCoord> Sample2EdgeEnpoints(int radius) {
	int side_start = std::rand() % 6;
	int side_end = (side_start + 3) % 6;
	TileCoord start = RandomEdgeTileFrom(side_start, radius);
	TileCoord end = RandomEdgeTileFrom(side_end, radius);
	return { start, end };
}
} // namespace


std::vector<TileCoord> MapManager::SampleRandomPoints(int count, int radius, int min_dist) {
    std::vector<TileCoord> points;
    int attempts = 0;
    const int max_attempts = 1000;

    while (points.size() < count && attempts < max_attempts) {
        attempts++;

        int q = std::rand() % (2 * radius + 1) - radius;
        int r1 = std::max(-radius, -q - radius);
        int r2 = std::min(radius, -q + radius);
        int r = std::rand() % (r2 - r1 + 1) + r1;

        TileCoord candidate{q, r};

        if (!free_tiles_.contains(candidate)) continue;

        bool too_close = false;
        for (const auto& p : points) {
            if (candidate.DistanceTo(p) < min_dist) {
                too_close = true;
                break;
            }
        }
        if (too_close) continue;
        points.push_back(candidate);
    }
    return points;
}

std::vector<TileCoord> MapManager::GeneratePathBetween(const TileCoord start, const TileCoord end) {
	std::unordered_set<TileCoord, std::hash<TileCoord>> tiles;
	std::vector<TileCoord> path;
    TileCoord current = start;

    while (current.q != end.q || current.r != end.r) {
        tiles.insert(current);
		path.push_back(current);
        std::vector<TileCoord> possible_moves;

        for (auto n : neighbors) {
            TileCoord next = { current.q + n.q, current.r + n.r };
            if (coords_to_tiles_.find(next) == coords_to_tiles_.end()) continue;

            int neighbours_count = 0;
            for (auto nn : neighbors) {
                TileCoord neighbor_tile = { next.q + nn.q, next.r + nn.r };
                if (tiles.count(neighbor_tile)) neighbours_count++;
            }

            if (neighbours_count < 2) {
                possible_moves.push_back(next);
            }
        }

        if (possible_moves.empty()) {
			// Reattempt river generation if stuck
			return std::move(GeneratePathBetween(start, end));
        }
		std::sort(possible_moves.begin(), possible_moves.end(), [&](const TileCoord& a, const TileCoord& b) {
			float da_end = glm::length(glm::vec2(a.q - end.q, a.r - end.r));
			float db_end = glm::length(glm::vec2(b.q - end.q, b.r - end.r));

			float da_center = glm::length(glm::vec2(a.q, a.r));
			float db_center = glm::length(glm::vec2(b.q, b.r));

			// Favor tiles closer to center
			float score_a = da_end - 0.1f * da_center;
			float score_b = db_end - 0.1f * db_center;

			return score_a < score_b;
		});

		// 75% chance to pick the best move, 25% random among valid moves
        size_t choice = (std::rand() % 4 == 0) ? std::rand() % possible_moves.size() : 0;
        current = possible_moves[choice];
    }
	path.push_back(end);
	return std::move(path);
}

void MapManager::PlaceRails() {
	std::cout << "Placing rails on the map..." << std::endl;
	for (const auto& [node, neighbors] : track_graph_) {
		TileCoord current = node;
		std::cout << "Placing rails at TileCoord (" << current.q << ", " << current.r << ")\n" << std::endl;

		for (const auto& neighbor : neighbors) {
			core::ecs::Entity rail_entity = ecs_manager_.CreateEntity();
			core::attributes::StaticMesh rail_mesh;
			std::cout << "  Created rail entity with ID: " << rail_entity.id << std::endl;
			rail_mesh.model_id = tile_models_["rail_simple"];
			ecs_manager_.AddAttribute<core::attributes::StaticMesh>(rail_entity.id, rail_mesh);
			std::cout << "  Rail to neighbor TileCoord (" << neighbor.q << ", " << neighbor.r << ")\n";

			core::attributes::Transform rail_transform;
			auto it_current = coords_to_tiles_.find(current);
			auto it_neighbor = coords_to_tiles_.find(neighbor);

			Transform& current_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(it_current->second.id);
			Transform& neighbor_transform = ecs_manager_.GetAttribute<core::attributes::Transform>(it_neighbor->second.id);
			rail_transform.position = (neighbor_transform.position + current_transform.position) / 2.0f;
			rail_transform.position.y = 10.0f;
			rail_transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
			TileCoord from = current;;
			TileCoord to = neighbor;
			GeoPos towards = GetGeoPosBetween(from, to);
			rail_transform.rotation.y = GetRotationByGeoPos(towards);
			ecs_manager_.AddAttribute<core::attributes::Transform>(rail_entity.id, rail_transform);
		}
	}
}

void MapManager::GenerateTracks(int radius) {
    std::vector<TileCoord> interest_points = SampleRandomPoints(4, radius, 7);
	std::shuffle(interest_points.begin(), interest_points.end(),
		std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

    for (size_t i = 0; i < interest_points.size(); ++i) {
        TileCoord start = interest_points[i];
        TileCoord end   = interest_points[(i + 1) % interest_points.size()];

		std::vector<TileCoord> track_tiles = GeneratePathBetween(start, end);
		if (track_tiles.size() < 2) continue;

		for (const auto& tc : track_tiles) {
			if (!track_graph_.contains(tc)) {
				track_graph_[tc] = {};
			}
		}

		track_graph_[track_tiles.front()].push_back(track_tiles[1]);
		track_graph_[track_tiles.back()].push_back(track_tiles[track_tiles.size() - 2]);

		TileCoord prev, current, next;
		for (size_t i = 1; i < track_tiles.size() - 1; ++i) {
			prev = track_tiles[i - 1];
			current = track_tiles[i];
			next = track_tiles[i + 1];
			track_graph_[current].push_back(prev);
			track_graph_[current].push_back(next);
			GeoPos start_pos = GetGeoPosBetween(current, prev);
			GeoPos end_pos = GetGeoPosBetween(current, next);

			if (start_pos == GeoPos::kNW && end_pos == GeoPos::kE ||
				start_pos == GeoPos::kE && end_pos == GeoPos::kNW) {
				placed_tracks_[current] = TrackType::kNW_E;
			} else if (start_pos == GeoPos::kNW && end_pos == GeoPos::kSE ||
					   start_pos == GeoPos::kSE && end_pos == GeoPos::kNW) {
				placed_tracks_[current] = TrackType::kNW_SE;
			} else if (start_pos == GeoPos::kSW && end_pos == GeoPos::kE ||
					   start_pos == GeoPos::kE && end_pos == GeoPos::kSW) {
				placed_tracks_[current] = TrackType::kSW_E;
			} else if (start_pos == GeoPos::kSW && end_pos == GeoPos::kNE ||
					   start_pos == GeoPos::kNE && end_pos == GeoPos::kSW) {
				placed_tracks_[current] = TrackType::kSW_NE;
			} else if ((start_pos == GeoPos::kW && end_pos == GeoPos::kE) ||
					   (start_pos == GeoPos::kE && end_pos == GeoPos::kW)) {
				placed_tracks_[current] = TrackType::kW_E;
			} else if ((start_pos == GeoPos::kW && end_pos == GeoPos::kNE) ||
					   (start_pos == GeoPos::kNE && end_pos == GeoPos::kW)) {
				placed_tracks_[current] = TrackType::kW_NE;
			} else if ((start_pos == GeoPos::kW && end_pos == GeoPos::kSE) ||
					   (start_pos == GeoPos::kSE && end_pos == GeoPos::kW)) {
				placed_tracks_[current] = TrackType::kW_SE;
			} else if ((start_pos == GeoPos::kNE && end_pos == GeoPos::kSE) ||
					   (start_pos == GeoPos::kSE && end_pos == GeoPos::kNE)) {
				placed_tracks_[current] = TrackType::kNE_SE;
			} else if ((start_pos == GeoPos::kNW && end_pos == GeoPos::kSW) ||
					   (start_pos == GeoPos::kSW && end_pos == GeoPos::kNW)) {
				placed_tracks_[current] = TrackType::kNW_SW;
			} else {
				// Handle unexpected cases
				std::cout << "Unexpected track orientation from "
						  << static_cast<int>(start_pos) << " to "
						  << static_cast<int>(end_pos) << std::endl;
			}

			auto it = coords_to_tiles_.find(current);
			if (it != coords_to_tiles_.end()) {
				Entity tile_entity = it->second;
				StaticMesh& static_mesh = ecs_manager_.GetAttribute<StaticMesh>(tile_entity.id);
				static_mesh.model_id = tile_models_[track_to_model[placed_tracks_[current]]];
				free_tiles_.erase(current);
			}
		}
		{
			auto it = coords_to_tiles_.find(start);
			if (it != coords_to_tiles_.end()) {
				Entity tile_entity = it->second;
				StaticMesh& static_mesh = ecs_manager_.GetAttribute<StaticMesh>(tile_entity.id);
				static_mesh.model_id = tile_models_["debug_tile_station"];
				free_tiles_.erase(current);
			}
		}
		{
			auto it = coords_to_tiles_.find(end);
			if (it != coords_to_tiles_.end()) {
				Entity tile_entity = it->second;
				StaticMesh& static_mesh = ecs_manager_.GetAttribute<StaticMesh>(tile_entity.id);
				static_mesh.model_id = tile_models_["debug_tile_station"];
				free_tiles_.erase(current);
			}
		}

	}
}

void MapManager::GenerateRiver(int radius) {
	// Generate 2 endpoints on the edge of the map, on opposite sides.
    auto [start, end] = Sample2EdgeEnpoints(radius);

	std::vector<TileCoord> river_tiles = GeneratePathBetween(start, end);

	// TODO: Add orienting logic based on river flow direction

	for (const auto& coord : river_tiles) {
		auto it = coords_to_tiles_.find(coord);
		if (it != coords_to_tiles_.end()) {
			Entity tile_entity = it->second;
			StaticMesh& static_mesh = ecs_manager_.GetAttribute<StaticMesh>(tile_entity.id);
			static_mesh.model_id = tile_models_["debug_tile_river"];
			free_tiles_.erase(coord);
		}
	}
}

void MapManager::GenerateBase(int radius) {
	Transform transform;
	transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);

	for (int q = -radius; q <= radius; ++q) {
		int r1 = std::max(-radius, -q - radius);
		int r2 = std::min(radius, -q + radius);
		for (int r = r1; r <= r2; ++r) {
			TileCoord coord{q, r};
			glm::vec3 world_pos = coord.ToWorldPosition();

			Entity tile_entity = ecs_manager_.CreateEntity();
			transform.position = world_pos;
			ecs_manager_.AddAttribute<Transform>(tile_entity.id, transform);
			StaticMesh static_mesh;
			static_mesh.model_id = tile_models_["debug_tile_empty"];
			ecs_manager_.AddAttribute<StaticMesh>(tile_entity.id, static_mesh);

			coords_to_tiles_.insert({coord, tile_entity});
			free_tiles_.insert(coord);
		}
	}
}

void MapManager::GenerateMap(int radius) {
	srand(static_cast<unsigned int>(time(nullptr)));
	LoadTileModels();
	GenerateBase(radius);
	GenerateRiver(radius);
	GenerateTracks(radius);
	PlaceRails();
}

void MapManager::LoadTileModels() {
	auto model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_empty/debug_tile_empty.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_empty"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_river/debug_tile_river.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_river"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_nv_e/debug_tile_nv_e.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_nv_e"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_nv_se/debug_tile_nv_se.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_nv_se"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_sv_e/debug_tile_sv_e.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_sv_e"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_sv_ne/debug_tile_sv_ne.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_sv_ne"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_v_e/debug_tile_v_e.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_v_e"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_v_ne/debug_tile_v_ne.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_v_ne"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_v_se/debug_tile_v_se.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_v_se"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_nv_sv/debug_tile_nv_sv.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_nv_sv"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_ne_se/debug_tile_ne_se.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_ne_se"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Tiles/Debug/debug_tile_station/debug_tile_station.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["debug_tile_station"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}

	model_res = asset_loader_.GetModelByPath("Rails/rail_simple/rail_simple.obj");
	if (model_res.has_value()) {
		Model& model = *(model_res.value());
		std::cout << "Model loaded with ID: " << model.id << std::endl;
		asset_loader_.LoadModel(model);
		renderer_.LoadModel(model);
		tile_models_["rail_simple"] = model.id;
	} else {
		std::cout << "Model not found!" << std::endl;
	}
}
} // namespace trains::managers
