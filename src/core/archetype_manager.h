#ifndef CORE_ARCHETYPE_MANAGER_H
#define CORE_ARCHETYPE_MANAGER_H

#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

#include "archetype.h"
#include "types.h"

namespace Core {

class ArchetypeManager {
public:
	explicit ArchetypeManager();

	// Registers a new attribute type with its size.
	// If attribute type already exists, it overrides its size.
	void RegisterAttributeType(AttributeType attribute_type, size_t attribute_size);

	// Adds an entity to the archetype matching the given signature. May create a new archetype if
	// necessary.
	void AddEntity(EntityID entity_id, const ArchetypeSignature& signature);
	// Removes an entity from the archetype matching the given signature.
	void RemoveEntity(EntityID entity_id);
	// Updates an entity's archetype from old_signature to new_signature.
	void UpdateEntityArchetype(EntityID entity_id, const ArchetypeSignature& new_signature);


	// Retrieves the attribute of the specified type for the given entity, if it exists.
	std::optional<std::reference_wrapper<IAttribute>> GetAttribute(
			EntityID entity_id, AttributeType attribute_type);
	// Sets the attribute of the specified type for the given entity. If the entity or attribute type
	// does not exist, throws an exception.
	void SetAttribute(EntityID entity_id, AttributeType attribute_type, IAttribute& attribute);

	// Queries all archetypes that match the given signature (i.e., contain at least the attributes
	// specified in the signature).
	std::vector<std::reference_wrapper<Archetype>> QueryArchetypes(
			const ArchetypeSignature& signature);
private:
	// Retrieves an existing archetype matching the signature or creates a new one if it doesn't
	// exist.
	std::reference_wrapper<Archetype> GetOrCreateArchetype(const ArchetypeSignature& signature);
	// Retrieves the archetype an entity belongs to. Returns std::nullopt if the entity is not
	// found.
	std::optional<std::reference_wrapper<Archetype>> GetEntityArchetype(EntityID entity_id);

private:
	// Maps attribute types to their sizes. Used when creating new archetypes for in-chunk attribute
	// delimitation.
	std::unordered_map<AttributeType, size_t> attribute_type_to_size_;
	// Maps entity IDs to their corresponding archetypes. Used for quick lookup of an entity's
	// archetype.
	std::unordered_map<EntityID, std::reference_wrapper<Archetype>> entity_to_archetype_;
	// Maps archetype signatures to their corresponding archetype instances.
	std::unordered_map<ArchetypeSignature, std::unique_ptr<Archetype>> signature_to_archetypes_;
};
} // namespace Core

#endif // CORE_ARCHETYPE_MANAGER_H
