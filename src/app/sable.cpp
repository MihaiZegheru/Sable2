#include <iostream>
#include <cassert>

#include "core/entity_manager.h"
#include "core/archetype.h"

// Test attribute types
struct Position : Core::IAttribute {
	float x, y, z;
	Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

struct Velocity : Core::IAttribute {
	float dx, dy, dz;
	Velocity(float dx = 0.0f, float dy = 0.0f, float dz = 0.0f) : dx(dx), dy(dy), dz(dz) {}
};

void TestArchetype() {
	std::cout << "=== Testing Archetype ===" << std::endl;

	// Create archetype with Position and Velocity
	Core::ArchetypeSignature sig;
	sig.set(0); // Position at index 0
	sig.set(1); // Velocity at index 1

	std::vector<size_t> sizes = {sizeof(Position), sizeof(Velocity)};
	std::vector<Core::AttributeType> types = {0, 1};

	Core::Archetype archetype(sig, sizes, types);
	std::cout << "Created archetype with Position and Velocity" << std::endl;

	// Add entities
	Core::EntityID entity1 = 100;
	Core::EntityID entity2 = 200;

	size_t idx1 = archetype.AddEntity(entity1);
	size_t idx2 = archetype.AddEntity(entity2);
	std::cout << "Added entity " << entity1 << " at index " << idx1 << std::endl;
	std::cout << "Added entity " << entity2 << " at index " << idx2 << std::endl;

	// Set attributes for entity1
	Position pos1(1.0f, 2.0f, 3.0f);
	Velocity vel1(0.1f, 0.2f, 0.3f);
	archetype.SetAttribute(entity1, 0, pos1);
	archetype.SetAttribute(entity1, 1, vel1);
	std::cout << "Set attributes for entity " << entity1 << std::endl;

	// Set attributes for entity2
	Position pos2(10.0f, 20.0f, 30.0f);
	Velocity vel2(1.0f, 2.0f, 3.0f);
	archetype.SetAttribute(entity2, 0, pos2);
	archetype.SetAttribute(entity2, 1, vel2);
	std::cout << "Set attributes for entity " << entity2 << std::endl;

	// Get and verify attributes
	Position& retrieved_pos1 = static_cast<Position&>(archetype.GetAttribute(entity1, 0));
	Velocity& retrieved_vel1 = static_cast<Velocity&>(archetype.GetAttribute(entity1, 1));

	std::cout << "Entity " << entity1 << " - Position: ("
			  << retrieved_pos1.x << ", " << retrieved_pos1.y << ", " << retrieved_pos1.z << ")" << std::endl;
	std::cout << "Entity " << entity1 << " - Velocity: ("
			  << retrieved_vel1.dx << ", " << retrieved_vel1.dy << ", " << retrieved_vel1.dz << ")" << std::endl;

	assert(retrieved_pos1.x == 1.0f && retrieved_pos1.y == 2.0f && retrieved_pos1.z == 3.0f);
	assert(retrieved_vel1.dx == 0.1f && retrieved_vel1.dy == 0.2f && retrieved_vel1.dz == 0.3f);

	Position& retrieved_pos2 = static_cast<Position&>(archetype.GetAttribute(entity2, 0));
	Velocity& retrieved_vel2 = static_cast<Velocity&>(archetype.GetAttribute(entity2, 1));

	std::cout << "Entity " << entity2 << " - Position: ("
			  << retrieved_pos2.x << ", " << retrieved_pos2.y << ", " << retrieved_pos2.z << ")" << std::endl;
	std::cout << "Entity " << entity2 << " - Velocity: ("
			  << retrieved_vel2.dx << ", " << retrieved_vel2.dy << ", " << retrieved_vel2.dz << ")" << std::endl;

	assert(retrieved_pos2.x == 10.0f && retrieved_pos2.y == 20.0f && retrieved_pos2.z == 30.0f);
	assert(retrieved_vel2.dx == 1.0f && retrieved_vel2.dy == 2.0f && retrieved_vel2.dz == 3.0f);

	// Test removal
	archetype.RemoveEntity(entity1);
	std::cout << "Removed entity " << entity1 << std::endl;

	// Verify entity2 still works after removal
	Position& pos_after_remove = static_cast<Position&>(archetype.GetAttribute(entity2, 0));
	std::cout << "After removal, entity " << entity2 << " - Position: ("
			  << pos_after_remove.x << ", " << pos_after_remove.y << ", " << pos_after_remove.z << ")" << std::endl;

	std::cout << "=== All Archetype Tests Passed! ===" << std::endl << std::endl;
}

void TestArchetypeErrors() {
	std::cout << "=== Testing Archetype Error Handling ===" << std::endl;

	// Test 1: Constructor with mismatched sizes and types
	std::cout << "Test: Constructor with size/type mismatch...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0, 1}; // Mismatch!
		Core::Archetype archetype(sig, sizes, types);
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 2: Constructor with entity stride exceeding chunk size
	std::cout << "Test: Constructor with stride exceeding chunk size...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {Core::kChunkSize + 1}; // Too large!
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 3: GetAttribute for non-existent entity
	std::cout << "Test: GetAttribute for non-existent entity...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);

		Core::EntityID non_existent = 999;
		archetype.GetAttribute(non_existent, 0);
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 4: GetAttribute for non-existent attribute type
	std::cout << "Test: GetAttribute for non-existent attribute type...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);

		Core::EntityID entity = 100;
		archetype.AddEntity(entity);
		archetype.GetAttribute(entity, 99); // Non-existent attribute type
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 5: RemoveEntity for non-existent entity
	std::cout << "Test: RemoveEntity for non-existent entity...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);

		Core::EntityID non_existent = 999;
		archetype.RemoveEntity(non_existent);
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 6: SetAttribute for non-existent entity
	std::cout << "Test: SetAttribute for non-existent entity...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);

		Core::EntityID non_existent = 999;
		Position pos(1.0f, 2.0f, 3.0f);
		archetype.SetAttribute(non_existent, 0, pos);
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	// Test 7: SetAttribute for non-existent attribute type
	std::cout << "Test: SetAttribute for non-existent attribute type...";
	try {
		Core::ArchetypeSignature sig;
		sig.set(0);
		std::vector<size_t> sizes = {sizeof(Position)};
		std::vector<Core::AttributeType> types = {0};
		Core::Archetype archetype(sig, sizes, types);

		Core::EntityID entity = 100;
		archetype.AddEntity(entity);
		Position pos(1.0f, 2.0f, 3.0f);
		archetype.SetAttribute(entity, 99, pos); // Non-existent attribute type
		std::cerr << " FAILED - No exception thrown!" << std::endl;
		assert(false);
	} catch (const std::runtime_error& e) {
		std::cout << " PASSED - Caught: " << e.what() << std::endl;
	}

	std::cout << "=== All Error Handling Tests Passed! ===" << std::endl << std::endl;
}

int main() {
	try {
		// Test EntityManager
		std::cout << "=== Testing EntityManager ===" << std::endl;
		Core::EntityManager entity_manager;
		auto entity = entity_manager.CreateEntity();
		if (!entity) {
			std::cerr << "Failed to create entity!" << std::endl;
			return 1;
		}
		std::cout << "Created entity with ID: " << entity->id << std::endl;
		std::cout << "=== EntityManager Tests Passed! ===" << std::endl << std::endl;

		// Test Archetype
		TestArchetype();

		// Test Error Handling
		TestArchetypeErrors();

		std::cout << "All tests completed successfully!" << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Test failed with exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
