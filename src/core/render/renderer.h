#ifndef CORE_RENDER_RENDERER_H
#define CORE_RENDER_RENDERER_H

#include <vector>
#include <unordered_map>

#include "drawable.h"
#include "drawablelight.h"
#include "render_model_data.h"
#include "core/graphics/model.h"
#include "core/ecs/entity.h"
#include "core/managers/shader_manager.h"
#include "core/managers/resource_manager.h"
#include "core/ecs/ecs_manager.h"

namespace core::render {

constexpr size_t kMaxLightsCount = 16;
extern GLuint PointLightBuffer;
extern void* mappedLightBuffer;

class Renderer {
public:
	static Renderer& GetInstance() {
		static Renderer instance;
		return instance;
	}

	void Draw(const std::vector<Drawable>& drawables,
			  const std::vector <DrawableLight>& lights,
			  ecs::EntityID camera_id);
	
private:
	Renderer();
	void InitShaders();
	void InitBuffers();

private:
	managers::ShaderManager& shader_manager_ = managers::ShaderManager::GetInstance();
	managers::ResourceManager& resource_manager_ = managers::ResourceManager::GetInstance();
	ecs::ECSManager& ecs_manager_ = ecs::ECSManager::GetInstance();
};
} // namespace core::render

#endif // CORE_RENDER_RENDERER_H
