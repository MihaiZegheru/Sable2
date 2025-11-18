#include "archetype_manager.h"

#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

#include "archetype.h"
#include "types.h"

namespace Core {

// Create an empty archetype for entities with no attributes.
ArchetypeManager::ArchetypeManager() {
	ArchetypeSignature empty_signature;
	signature_to_archetypes_[empty_signature] = std::make_unique<Archetype>(
													empty_signature,
													std::vector<size_t>{},
													std::vector<AttributeType>{});
}

void ArchetypeManager::RegisterAttributeType(AttributeType attribute_type, size_t attribute_size) {
	attribute_type_to_size_[attribute_type] = attribute_size;
}

std::reference_wrapper<Archetype> ArchetypeManager::GetOrCreateArchetype(
		const ArchetypeSignature& signature) {
	auto it = signature_to_archetypes_.find(signature);
	if (it != signature_to_archetypes_.end()) {
		return *(it->second);
	}

	std::vector<size_t> attribute_sizes;
	std::vector<AttributeType> attribute_types;

	for (size_t i = 0; i < kMaxAttributes; ++i) {
		if (signature.test(i)) {
			AttributeType attr_type = static_cast<AttributeType>(i);
			auto size_it = attribute_type_to_size_.find(attr_type);
			if (size_it == attribute_type_to_size_.end()) {
				throw std::runtime_error("Attribute type not registered in ArchetypeManager.");
			}
			attribute_types.push_back(attr_type);
			attribute_sizes.push_back(size_it->second);
		}
	}

	auto archetype = std::make_unique<Archetype>(signature, attribute_sizes, attribute_types);
	Archetype& archetype_ref = *archetype;
	signature_to_archetypes_[signature] = std::move(archetype);
	return archetype_ref;
}

void ArchetypeManager::AddEntity(EntityID entity_id, const ArchetypeSignature& signature) {
	auto archetype = GetOrCreateArchetype(signature);
	archetype.get().AddEntity(entity_id);
	entity_to_archetype_.insert({entity_id, archetype});
}

void ArchetypeManager::RemoveEntity(EntityID entity_id, const ArchetypeSignature& signature) {

	// TODO: We may want to free up archetypes if they become empty.

	auto it = signature_to_archetypes_.find(signature);
	if (it == signature_to_archetypes_.end()) {
		throw std::runtime_error("Archetype with given signature does not exist.");
	}
	it->second->RemoveEntity(entity_id);
	entity_to_archetype_.erase(entity_id);
}

void ArchetypeManager::UpdateEntityArchetype(EntityID entity_id,
		const ArchetypeSignature& old_signature,
		const ArchetypeSignature& new_signature) {
	RemoveEntity(entity_id, old_signature);
	AddEntity(entity_id, new_signature);
}

std::optional<std::reference_wrapper<Archetype>> ArchetypeManager::GetEntityArchetype(
		EntityID entity_id) {
	auto it = entity_to_archetype_.find(entity_id);
	if (it == entity_to_archetype_.end()) {
		return std::nullopt;
	}
	return it->second;
}

std::optional<std::reference_wrapper<IAttribute>> ArchetypeManager::GetAttribute(
		EntityID entity_id, AttributeType attribute_type) {
	auto archetype_opt = GetEntityArchetype(entity_id);
	if (!archetype_opt.has_value()) {
		return std::nullopt;
	}
	IAttribute& attribute = archetype_opt->get().GetAttribute(entity_id, attribute_type);
	return attribute;
}

void ArchetypeManager::SetAttribute(EntityID entity_id, AttributeType attribute_type,
		IAttribute& attribute) {
	auto archetype_opt = GetEntityArchetype(entity_id);
	if (!archetype_opt.has_value()) {
		throw std::runtime_error("Entity not found in any archetype.");
	}
	archetype_opt->get().SetAttribute(entity_id, attribute_type, attribute);
}

std::vector<std::reference_wrapper<Archetype>> ArchetypeManager::QueryArchetypes(
		const ArchetypeSignature& signature) {

	// TODO: Optimize by doing 2 binary searches on the sorted signatures, upper and lower bounds
	// and keep the interval.

	std::vector<std::reference_wrapper<Archetype>> archetypes;
	archetypes.reserve(signature_to_archetypes_.size());
	for (auto& [signature, archetype_ptr] : signature_to_archetypes_) {
		archetypes.push_back(*archetype_ptr);
	}
	return archetypes;
}
} // namespace Core