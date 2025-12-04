#ifndef PROJECTS_TRAINS_TYPES_RESOURCE_TYPE_H
#define PROJECTS_TRAINS_TYPES_RESOURCE_TYPE_H

namespace trains::types {

enum class ResourceType {
	kDefault = -1,
    kFood = 0,
    kWood = 1,
    kStone = 2,
    kGold = 3
};
} // namespace trains::types

#endif // PROJECTS_TRAINS_TYPES_RESOURCE_TYPE_H