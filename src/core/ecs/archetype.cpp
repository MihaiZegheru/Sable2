#include "archetype.h"

#include <functional>
#include <stdexcept>

namespace core::ecs {

Archetype::Archetype(ArchetypeSignature signature, const std::vector<size_t>& attribute_sizes,
					 const std::vector<AttributeType>& attribute_types) :
		signature_(signature),
		attribute_types_(attribute_types) {
	if (attribute_sizes.size() != attribute_types.size()) {
		throw std::runtime_error("Attribute sizes and types size mismatch.");
	}

	attribute_offsets_.resize(attribute_types_.size() + 1);
	attribute_offsets_[0] = 0;
	for (size_t i = 0; i < attribute_types_.size(); ++i) {
		attribute_type_to_index_[attribute_types_[i]] = i;
		attribute_offsets_[i + 1] = attribute_offsets_[i] + attribute_sizes[i];
	}
	entity_stride_ = attribute_offsets_.back();

	if (attribute_sizes.size() == 0) {
		entities_per_chunk_ = std::numeric_limits<size_t>::max();
	} else {
		entities_per_chunk_ = kChunkSize / entity_stride_;
	}
	if (entities_per_chunk_ == 0) {
		throw std::runtime_error("Archetype entity stride exceeds chunk size.");
	}
}

size_t Archetype::AddEntity(EntityID entity_id) {
	size_t index = entities_.size();
	size_t chunk_index = index / entities_per_chunk_;

	if (chunk_index >= chunks_.size()) {
		chunks_.emplace_back(std::make_unique<uint8_t[]>(kChunkSize));
	}
	entity_to_index_[entity_id] = index;
	entities_.push_back(entity_id);

	return index;
}

// TODO: Consider optimizing RemoveEntity's move.

// Swaps with the last entity to maintain contiguity.
// Moves the last entity's data into the removed entity's slot.
void Archetype::RemoveEntity(EntityID entity_id) {
	auto it = entity_to_index_.find(entity_id);
	if (it == entity_to_index_.end()) {
		throw std::runtime_error("Entity not found in archetype.");
	}

	size_t index = it->second;
	EntityID last_entity = entities_[entities_.size() - 1];
	entities_[index] = last_entity;
	entity_to_index_[last_entity] = index;
	MoveEntityData(index, entities_.size() - 1);

	entities_.pop_back();
	entity_to_index_.erase(it);
}

IAttribute& Archetype::GetAttribute(EntityID entity_id,
		AttributeType attribute_type) {
	auto it = entity_to_index_.find(entity_id);
	if (it == entity_to_index_.end()) {
		throw std::runtime_error("Entity not found in archetype.");
	}
	size_t entity_index = it->second;
	size_t chunk_index = entity_index / entities_per_chunk_;
	size_t index_in_chunk = entity_index % entities_per_chunk_;

	auto attr_it = attribute_type_to_index_.find(attribute_type);
	if (attr_it == attribute_type_to_index_.end()) {
		throw std::runtime_error("Attribute type not found in archetype.");
	}
	size_t attribute_offset = attribute_offsets_[attr_it->second];
	uint8_t* chunk = chunks_[chunk_index].get();
	uint8_t* attribute_ptr = chunk + (index_in_chunk * entity_stride_) + attribute_offset;

	IAttribute* attribute = reinterpret_cast<IAttribute*>(attribute_ptr);
	return *attribute;
}

void Archetype::SetAttribute(EntityID entity_id, AttributeType attribute_type,
							 IAttribute& attribute) {
	IAttribute& attr = GetAttribute(entity_id, attribute_type);
	auto attr_it = attribute_type_to_index_.find(attribute_type);
	if (attr_it == attribute_type_to_index_.end()) {
		throw std::runtime_error("Attribute type not found in archetype.");
	}

	size_t attribute_size = attribute_offsets_[attr_it->second + 1] -
							attribute_offsets_[attr_it->second];
	std::memcpy(&attr, &attribute, attribute_size);
}

void Archetype::MoveEntityData(size_t dest_index, size_t src_index) {
	if (dest_index == src_index) return;

	size_t dest_chunk_index = dest_index / entities_per_chunk_;
	size_t dest_index_in_chunk = dest_index % entities_per_chunk_;
	size_t src_chunk_index = src_index / entities_per_chunk_;
	size_t src_index_in_chunk = src_index % entities_per_chunk_;

	uint8_t* dest_chunk = chunks_[dest_chunk_index].get();
	uint8_t* src_chunk = chunks_[src_chunk_index].get();

	uint8_t* dest_ptr = dest_chunk + (dest_index_in_chunk * entity_stride_);
	uint8_t* src_ptr = src_chunk + (src_index_in_chunk * entity_stride_);

	std::memcpy(dest_ptr, src_ptr, entity_stride_);
}
} // namespace core::ecs
