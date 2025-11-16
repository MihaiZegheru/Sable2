#ifndef CORE_ARCHETYPE_H
#define CORE_ARCHETYPE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

#include "entity.h"
#include "types.h"

namespace Core {

// Size of each archetype data chunk in bytes (16 KB).
static constexpr size_t kChunkSize = 16 << 10;

// Archetype stores all entities that share the same attribute signature.
// Uses chunk-based storage for cache-friendly iterations.
class Archetype {
public:
	explicit Archetype(ArchetypeSignature signature, const std::vector<size_t>& attribute_sizes,
					   const std::vector<AttributeType>& attribute_types);
	
	// Adds an entity to the archetype and returns its index within the archetype.
	// Function will allocate new chunk if necessary.
	size_t AddEntity(EntityID entity_id);
	// Removes an entity from the archetype.
	void RemoveEntity(EntityID entity_id);

	IAttribute& GetAttribute(EntityID entity_id,
																   AttributeType attribute_type);
	void SetAttribute(EntityID entity_id, AttributeType attribute_type, IAttribute& attribute);

private:
	// Signature representing the set of attributes for this archetype.
	ArchetypeSignature signature_;
	
	// Types of attributes stored in this archetype.
	std::vector<AttributeType> attribute_types_;
	// Map from attribute type to its index in the attribute_types_ vector. Used for fast internal
	// management.
	std::unordered_map<AttributeType, size_t> attribute_type_to_index_;
	// Offsets of each attribute type within an entity's data block. This is calculated in the
	// constructor based on the attribute sizes provided by computing the partial sums.
	std::vector<size_t> attribute_offsets_;

	// Stride (in bytes) of a single entity's data block within a chunk.
	size_t entity_stride_;
	// Number of entities that can fit in a single chunk.
	size_t entities_per_chunk_;
	// List of entities in this archetype.
	std::vector<EntityID> entities_;

	std::unordered_map<EntityID, size_t> entity_to_index_;

	std::vector<std::unique_ptr<uint8_t[]>> chunks_;


};
} // namespace Core

#endif // CORE_ARCHETYPE_H
