#include <iostream>

#include "core/entity_manager.h"

int main() {
	Core::EntityManager entity_manager;
	auto entity = entity_manager.CreateEntity();
	if (!entity) {
		return 1;
	}
	std::cout << "Created entity with ID: " << entity->id << std::endl;
	return 0;
}
