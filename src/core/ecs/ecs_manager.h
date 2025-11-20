#ifndef CORE_ECS_MANAGER_H
#define CORE_ECS_MANAGER_H

#include <cstddef>
#include <typeindex>
#include <unordered_map>

#include "archetype_manager.h"
#include "entity.h"
#include "entity_manager.h"
#include "system.h"
#include "system_manager.h"
#include "types.h"

namespace core::ecs {

// TODO: This should be the only interaction point for ECS operations. Therefore, the other managers
// should have their methods private and be friends with this class.

class ECSManager {
public:
	static ECSManager& GetInstance() {
		static ECSManager instance;
		return instance;
	}

	// Create a new entity and return it.
	Entity CreateEntity();
	// Destroy an entity.
	void DestroyEntity(EntityID entity);

	// Registers a system of type T with the given archetype signature.
	// T must be derived from System.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<System, T>>>
	void RegisterSystem(ArchetypeSignature& signature) {
		system_manager_.RegisterSystem<T>(signature);
	}
	// Updates all registered systems.
	void UpdateSystems(float delta_time) {
		system_manager_.UpdateSystems(delta_time);
	}

	// Registers an attribute type T with its size. This must be called before using the attribute
	// in any entity.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	void RegisterAttribute() {
		std::type_index type(typeid(T));
        if (component_types_.find(type) == component_types_.end()) {
			archetype_manager_.RegisterAttributeType(next_attribute_type_, sizeof(T));
            component_types_[type] = next_attribute_type_++;
        }
	}
	// Adds an attribute of type T to the specified entity.
	// If attribute of that type already exists, it returns without changes.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	void AddAttribute(EntityID entity, T& attribute) {
		AttributeType type = GetAttributeType<T>();

		ArchetypeSignature old_signature = entity_manager_.GetEntitySignature(entity);
		if (old_signature.test(type)) {
			return;
		}
		ArchetypeSignature new_signature = old_signature;
		new_signature.set(type);

		// Update archetype
		archetype_manager_.UpdateEntityArchetype(entity, old_signature, new_signature);
		entity_manager_.SetEntitySignature(entity, new_signature);

		// Set attribute data
		archetype_manager_.SetAttribute(entity, type, attribute);

		// TODO: Might want to update systems as well.
	}
	// Removes an attribute of type T from the specified entity.
	// If attribute of that type does not exist, it returns without changes.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	void RemoveAttribute(EntityID entity) {
		AttributeType type = GetAttributeType<T>();

		ArchetypeSignature old_signature = entity_manager_.GetEntitySignature(entity);
		if (!old_signature.test(type)) {
			return;
		}
		ArchetypeSignature new_signature = old_signature;
		new_signature.reset(type);

		// Update archetype
		archetype_manager_.UpdateEntityArchetype(entity.id, new_signature);
		entity_manager_.SetEntitySignature(entity, new_signature);

		// TODO: Might want to update systems as well.
	}
	// Retrieves the attribute of type T for the specified entity.
	// Throws an exception if the attribute does not exist.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	T& GetAttribute(EntityID entity) {
		AttributeType type = GetAttributeType<T>();
		auto attribute_opt = archetype_manager_.GetAttribute(entity, type);
		if (!attribute_opt.has_value()) {
			throw std::runtime_error("Attribute not found for entity.");
		}
		return static_cast<T&>(attribute_opt->get());
	}
	// Checks if the specified entity has an attribute of type T.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	bool HasAttribute(EntityID entity) {
		AttributeType type = GetAttributeType<T>();
		ArchetypeSignature signature = entity_manager_.GetEntitySignature(entity);
		return signature.test(type);
	}

	inline ArchetypeManager& GetArchetypeManager() {
		return archetype_manager_;
	}
	inline EntityManager& GetEntityManager() {
		return entity_manager_;
	}
	inline SystemManager& GetSystemManager() {
		return system_manager_;
	}

private:
	ECSManager() = default;

	// Retrieves the AttributeType for a given attribute class T.
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IAttribute, T>>>
	AttributeType GetAttributeType() {
		auto it = component_types_.find(typeid(T));
		if (it == component_types_.end()) {
			throw std::runtime_error("Attribute type not registered.");
		}
		return it->second;
	}

private:
	ArchetypeManager archetype_manager_;
	EntityManager entity_manager_;
	SystemManager system_manager_;

	// Maps C++ type indices to AttributeType identifiers.
	std::unordered_map<std::type_index, AttributeType> component_types_;

	// Next available AttributeType identifier.
	AttributeType next_attribute_type_ = 0;
};
} // namespace core::ecs

#endif // CORE_ECS_MANAGER_H
