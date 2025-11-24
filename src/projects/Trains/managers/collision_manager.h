#ifndef PROJECTS_TRAINS_MANAGERS_COLLISION_MANAGER_H
#define PROJECTS_TRAINS_MANAGERS_COLLISION_MANAGER_H

#include <unordered_set>
#include <unordered_map>

#include "projects/Trains/types/collision.h"
#include "core/ecs/types.h"
#include "core/ecs/ecs_manager.h"
#include "core/attributes/transform.h"
#include "projects/Trains/attributes/box_collider.h"

#include <iostream>

namespace trains::managers {

// TODO: Should create a collision world to manage multiple collision spaces and have 
// predicatble entities.

class CollisionManager {
public:
    static CollisionManager& GetInstance() {
		static CollisionManager instance;
		return instance;
	}

	void AddCollidable(core::ecs::EntityID entity_id) {
		collidables_.push_back(entity_id);
	}

	// TODO: Temporary fix for entity destruction.

	void RemoveCollidable(core::ecs::EntityID entity_id) {
		collidables_.erase(std::remove(collidables_.begin(), collidables_.end(), entity_id), collidables_.end());
		deleted.insert(entity_id);
		// Also remove from collisions
		collisions_.erase(entity_id);
		for (auto& [other_entity, collision_set] : collisions_) {
			for (auto it = collision_set.begin(); it != collision_set.end(); ) {
				if (it->entityA == entity_id || it->entityB == entity_id) {
					it = collision_set.erase(it);
				} else {
					++it;
				}
			}
		}
		collisions_cache_.clear();
		collisions_.clear();
	}

	void AddCollision(const types::Collision& collision) {
		collisions_[collision.entityA].insert(collision);
		collisions_[collision.entityB].insert(collision);
	}

	void GetCollisionsForEntity(core::ecs::EntityID entity_id,
							 std::unordered_set<types::Collision, std::hash<types::Collision>>& out_collisions) {
		if (collisions_cache_.find(entity_id) != collisions_cache_.end()) {
			out_collisions = collisions_cache_[entity_id];
		} else {
			out_collisions.clear();
		}
	}
	
	void ClearCollisions() {
		collisions_cache_.swap(collisions_);
		collisions_.clear();
	}

	void ComputeCollisions() {
		// This is extremely inefficient because I dont save AABB data structures. ;(

		for (size_t i = 0; i < collidables_.size(); ++i) {
			for (size_t j = i + 1; j < collidables_.size(); ++j) {
				core::ecs::EntityID entityA = collidables_[i];
				core::ecs::EntityID entityB = collidables_[j];
				if (entityA == entityB) continue;
				if (deleted.contains(entityA) || deleted.contains(entityB)) {
					continue;
				}

				// Simple AABB collision detection
				trains::attributes::BoxCollider& box_colliderA = ecs_manager_.GetAttribute<trains::attributes::BoxCollider>(entityA);
				trains::attributes::BoxCollider& box_colliderB = ecs_manager_.GetAttribute<trains::attributes::BoxCollider>(entityB);
				core::attributes::Transform& transformA = ecs_manager_.GetAttribute<core::attributes::Transform>(entityA);
				core::attributes::Transform& transformB = ecs_manager_.GetAttribute<core::attributes::Transform>(entityB);
				
				bool collisionX = transformA.position.x + box_colliderA.size.x / 2 >= transformB.position.x - box_colliderB.size.x / 2 &&
								  transformB.position.x + box_colliderB.size.x / 2 >= transformA.position.x - box_colliderA.size.x / 2;
				bool collisionY = transformA.position.y + box_colliderA.size.y / 2 >= transformB.position.y - box_colliderB.size.y / 2 &&
								  transformB.position.y + box_colliderB.size.y / 2 >= transformA.position.y - box_colliderA.size.y / 2;
				bool collisionZ = transformA.position.z + box_colliderA.size.z / 2 >= transformB.position.z - box_colliderB.size.z / 2 &&
								  transformB.position.z + box_colliderB.size.z / 2 >= transformA.position.z - box_colliderA.size.z / 2;

				if (collisionX && collisionY && collisionZ) {
					trains::types::Collision collision{entityA, entityB};
					AddCollision(collision);
				}
			}
		}
		collidables_.clear();
	}

private:
	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

private:
	core::ecs::ECSManager& ecs_manager_ = core::ecs::ECSManager::GetInstance();

	std::unordered_map<core::ecs::EntityID, 
					   std::unordered_set<types::Collision, std::hash<types::Collision>>,
					   std::hash<core::ecs::EntityID>> collisions_;
	std::unordered_map<core::ecs::EntityID, 
					std::unordered_set<types::Collision, std::hash<types::Collision>>,
					std::hash<core::ecs::EntityID>> collisions_cache_;

	std::vector<core::ecs::EntityID> collidables_;

	// I need a system run order asap...
	std::unordered_set<core::ecs::EntityID> deleted;
};

} // namespace trains::managers

#endif // PROJECTS_TRAINS_MANAGERS_COLLISION_MANAGER_H
