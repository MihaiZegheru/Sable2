#include "system.h"

#include "ecs_manager.h"

namespace core::ecs {
	
ISystem::ISystem()
	: ecs_manager_(ECSManager::GetInstance()),
	  archetype_manager_(ecs_manager_.GetArchetypeManager()) {}
} // namespace core::ecs