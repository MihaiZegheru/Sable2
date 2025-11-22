#ifndef PROJECTS_TRAINS_MANAGERS_MAP_MANAGER_H
#define PROJECTS_TRAINS_MANAGERS_MAP_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <functional>
#include <map>

#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>

#include "core/graphics/model.h"
#include "core/ecs/ecs_manager.h"
#include "core/assetloader/asset_loader_manager.h"
#include "core/render/renderer.h"
#include "core/ecs/types.h"

constexpr float kTileWidth = 28.0f;
constexpr float kTileHeight = 33.0f;

enum class GeoPos {
	kN,
	kS,
	kE,
	kW,
	kNE,
	kNW,
	kSE,
	kSW
};

enum class TrackType {
	kNW_E,
	kNW_SE,
	kSW_E,
	kSW_NE,
	kW_E,
	kW_NE,
	kW_SE,
	kNE_SE,
	kNW_SW
};

inline std::map<TrackType, std::string> track_to_model = {
	{ TrackType::kNW_E, "debug_tile_nv_e" },
	{ TrackType::kNW_SE, "debug_tile_nv_se" },
	{ TrackType::kSW_E, "debug_tile_sv_e" },
	{ TrackType::kSW_NE, "debug_tile_sv_ne" },
	{ TrackType::kW_E, "debug_tile_v_e" },
	{ TrackType::kW_NE, "debug_tile_v_ne" },
	{ TrackType::kW_SE, "debug_tile_v_se" },
	{ TrackType::kNE_SE, "debug_tile_ne_se" },
	{ TrackType::kNW_SW, "debug_tile_nv_sv" }
};

struct TileCoord {
	int q;
	int r;

	bool operator==(const TileCoord& other) const {
		return q == other.q && r == other.r;
	}

	glm::vec3 ToWorldPosition() const {
		float x = kTileWidth * (q + r * 0.5f);
		float z = kTileHeight * r * 3 / 4;
		return glm::vec3(x, 0.0f, z);
	}

	bool operator<(const TileCoord& other) const {
		return std::tie(q, r) < std::tie(other.q, other.r);
	}

	int DistanceTo(const TileCoord& other) const {
		return (std::abs(q - other.q)
		      + std::abs(q + r - other.q - other.r)
		      + std::abs(r - other.r)) / 2;
	}
};

namespace std {
    template<>
    struct hash<TileCoord> {
        std::size_t operator()(const TileCoord& tc) const noexcept {
            std::size_t h1 = std::hash<int>()(tc.q);
            std::size_t h2 = std::hash<int>()(tc.r);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}


namespace trains::managers {

class MapManager {
public:
	static MapManager& GetInstance() {
		static MapManager instance;
		return instance;
	}

	void GenerateMap(int radius);

	inline TileCoord GetStartingTile() const {
		return placed_tracks_.begin()->first; 
	}

	inline std::optional<std::vector<TileCoord>> GetNextTrackTiles(const TileCoord& current) const {
		auto it = track_graph_.find(current);
		if (it != track_graph_.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	inline core::ecs::Entity& GetTileEntityAt(const TileCoord& coord) const {
		auto it = coords_to_tiles_.find(coord);
		if (it != coords_to_tiles_.end()) {
			return const_cast<core::ecs::Entity&>(it->second);
		}
		throw std::runtime_error("TileCoord not found in coords_to_tiles_");
	}

	inline TrackType GetTrackTypeAt(const TileCoord& coord) const {
		auto it = placed_tracks_.find(coord);
		if (it != placed_tracks_.end()) {
			return it->second;
		}
		throw std::runtime_error("TileCoord not found in placed_tracks_");
	}

private:
	MapManager() = default;

	void LoadTileModels();

	void GenerateBase(int radius);
	void GenerateRiver(int radius);
	void GenerateTracks(int radius);
	void PlaceRails();

	std::vector<TileCoord> SampleRandomPoints(int count, int radius, int min_dist);
	std::vector<TileCoord> GeneratePathBetween(const TileCoord start, const TileCoord end);

private:
	std::unordered_map<std::string, size_t> tile_models_;
	std::unordered_map<TileCoord, core::ecs::Entity, std::hash<TileCoord>> coords_to_tiles_;
	std::unordered_set<TileCoord, std::hash<TileCoord>> free_tiles_;
	std::unordered_map<TileCoord, TrackType, std::hash<TileCoord>> placed_tracks_;

	std::unordered_map<TileCoord, std::vector<TileCoord>, std::hash<TileCoord>> track_graph_;

	core::ecs::ECSManager& ecs_manager_ = core::ecs::ECSManager::GetInstance();
	core::assetloader::AssetLoaderManager& asset_loader_ = core::assetloader::AssetLoaderManager::GetInstance();
	core::render::Renderer& renderer_ = core::render::Renderer::GetInstance();
};
} // namespace trains::managers

#endif // PROJECTS_TRAINS_MANAGERS_MAP_MANAGER_H
