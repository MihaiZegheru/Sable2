#ifndef CORE_MANAGERS_SCENE_MANAGER_H
#define CORE_MANAGERS_SCENE_MANAGER_H

#include "core/ecs/ecs_manager.h"
#include "core/ecs/types.h"

namespace core::managers {

class SceneManager {
public:
	static SceneManager& GetInstance() {
		static SceneManager instance;
		return instance;
	}

	inline void SetMainCamera(ecs::EntityID entity_id) {
		main_camera_ = entity_id;
	}

	inline ecs::EntityID GetMainCamera() const {
		return main_camera_;
	}

private:
	SceneManager() = default;

	ecs::EntityID main_camera_;
};
} // namespace core::managers

#endif // CORE_MANAGERS_SCENE_MANAGER_H
