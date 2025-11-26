#ifndef PROJECTS_TRAINS_MANAGERS_MINIMAP_MANAGER_H
#define PROJECTS_TRAINS_MANAGERS_MINIMAP_MANAGER_H

#include "core/ecs/types.h"

namespace trains::managers {

class MinimapManager {
public:
	static MinimapManager& GetInstance() {
		static MinimapManager instance;
		return instance;
	}

	inline void SetMinimapCameraEntityID(core::ecs::EntityID entity_id) {
		minimap_camera_entity_id_ = entity_id;
	}
	inline core::ecs::EntityID GetMinimapCameraEntityID() const {
		return minimap_camera_entity_id_;
	}
	
private:
	MinimapManager() = default;
	MinimapManager(const MinimapManager&) = delete;
	MinimapManager& operator=(const MinimapManager&) = delete;

private:
	core::ecs::EntityID minimap_camera_entity_id_;
};
} // namespace trains::managers

#endif // PROJECTS_TRAINS_MANAGERS_MINIMAP_MANAGER_H
