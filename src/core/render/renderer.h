#ifndef CORE_RENDER_RENDERER_H
#define CORE_RENDER_RENDERER_H

#include <vector>
#include <unordered_map>

#include "drawable.h"
#include "render_model_data.h"
#include "core/graphics/model.h"
#include "core/ecs/entity.h"

namespace core::render {

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
	void Draw(const std::vector<Drawable>& drawables, ecs::EntityID active_camera_id);
	
private:
	void InitShaders();
	Renderer();

private:
	std::unordered_map <size_t, RenderModelData> id_to_render_data_;

	
	GLuint default_shader_program_;
};
} // namespace core::render

#endif // CORE_RENDER_RENDERER_H
