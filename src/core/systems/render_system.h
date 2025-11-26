#ifndef CORE_SYSTEMS_RENDER_SYSTEM_H
#define CORE_SYSTEMS_RENDER_SYSTEM_H

#include "core/ecs/archetype.h"
#include "core/ecs/ecs_manager.h"
#include "core/ecs/system.h"
#include "core/render/renderer.h"

namespace core::systems {

class RenderSystem : public ecs::ISystem {
public:
	void Start() override;
	void StartAllArchetypes() override;
	void Tick(float delta_time) override;
	void TickAllArchetypes(float delta_time) override;

private:
	render::Renderer& renderer_ = render::Renderer::GetInstance();
};
} // namespace core::systems

# endif // CORE_SYSTEMS_RENDER_SYSTEM_H
