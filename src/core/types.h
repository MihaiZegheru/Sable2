#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <bitset>
#include <cstdint>

namespace Core {

// Define a type alias for Entity IDs.
using EntityID = uint16_t;

// Define a type alias for Attribute Types.
using AttributeType = uint8_t;
// Maximum number of components an entity can have. This value should be in powers of two for
// optimal use of possible number of attributes.
static constexpr size_t kMaxAttributes = 64;
// Archetype signature represented as a bitset, where each bit indicates the presence of an
// unique attribute.
using ArchetypeSignature = std::bitset<kMaxAttributes>;
// Base interface for all attribute types.
class IAttribute {
public:
	virtual ~IAttribute() = default;
};
} // namespace Core

#endif // CORE_TYPES_H
