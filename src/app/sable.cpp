#include <iostream>

#include "core/ecs/ecs_manager.h"
#include "core/ecs/types.h"

struct Transform : core::ecs::IAttribute {
	float x, y, z;
	Transform(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

struct Velocity : core::ecs::IAttribute {
	float vx, vy, vz;
	Velocity(float vx = 0.0f, float vy = 0.0f, float vz = 0.0f) : vx(vx), vy(vy), vz(vz) {}
};

class PhysicsSystem : public core::ecs::System {
public:
	void Tick(float delta_time) override {
		// Tick
	}

	void TickArchetype(core::ecs::Archetype& archetype, float delta_time) override {
		archetype.ForEach([this, delta_time, &archetype](core::ecs::EntityID entity_id, size_t index) {
			Transform& transform = ecs_manager_.GetAttribute<Transform>(entity_id);
			Velocity& velocity = ecs_manager_.GetAttribute<Velocity>(entity_id);

			std::cout << "Archetype with signature: " << static_cast<int>(archetype.GetSignature().to_ulong()) << "\n";

			std::cout << "Entity " << entity_id << ":" << index << " at ("
					  << transform.x << ", " << transform.y << ", " << transform.z << ") "
					  << "with velocity ("
					  << velocity.vx << ", " << velocity.vy << ", " << velocity.vz << ")\n";

			transform.x += velocity.vx * delta_time;
			transform.y += velocity.vy * delta_time;
			transform.z += velocity.vz * delta_time;

			std::cout << "Entity " << entity_id << " moved to ("
					  << transform.x << ", " << transform.y << ", " << transform.z << ")\n\n";
		});
	}
public:
	core::ecs::ECSManager& ecs_manager_ = core::ecs::ECSManager::GetInstance();
};

int main() {
	core::ecs::ECSManager& ecs_manager = core::ecs::ECSManager::GetInstance();
	ecs_manager.RegisterAttribute<Transform>();
	ecs_manager.RegisterAttribute<Velocity>();

	core::ecs::ArchetypeSignature physics_signature;
	physics_signature.set(0); // Transform
	physics_signature.set(1); // Velocity
	ecs_manager.RegisterSystem<PhysicsSystem>(physics_signature);

	core::ecs::Entity entity = ecs_manager.CreateEntity();
	Transform transform(0.0f, 0.0f, 0.0f);
	Velocity velocity(1.0f, 0.0f, 0.0f);
	ecs_manager.AddAttribute<Transform>(entity.id, transform);
	ecs_manager.AddAttribute<Velocity>(entity.id, velocity);

	core::ecs::Entity entity2 = ecs_manager.CreateEntity();
	Transform transform2(0.0f, 0.0f, 0.0f);
	Velocity velocity2(0.0f, 1.0f, 0.0f);
	ecs_manager.AddAttribute<Transform>(entity2.id, transform2);
	ecs_manager.AddAttribute<Velocity>(entity2.id, velocity2);

	float delta_time = 1.0f; // Simulate 1 second per tick
	for (int i = 0; i < 5; ++i) {
		std::cout << "Tick " << i + 1 << ":\n";
		ecs_manager.UpdateSystems(delta_time);
	}
}