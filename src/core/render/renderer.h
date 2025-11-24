#ifndef CORE_RENDER_RENDERER_H
#define CORE_RENDER_RENDERER_H

#include <vector>
#include <unordered_map>

#include "drawable.h"
#include "drawablelight.h"
#include "render_model_data.h"
#include "core/graphics/model.h"
#include "core/ecs/entity.h"

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
	
	void LoadModel(const graphics::Model& model);
	void UnloadModel(size_t model_id) { 
		// TODO 
	};
	void Draw(const std::vector<Drawable>& drawables,
			  const std::vector <DrawableLight>& lights,
			  ecs::EntityID active_camera_id);
	
private:
	Renderer();
	void InitShaders();
	void InitBuffers();

private:
	std::unordered_map <size_t, RenderModelData> id_to_render_data_;

	
	GLuint default_shader_program_;
};
} // namespace core::render

#endif // CORE_RENDER_RENDERER_H
