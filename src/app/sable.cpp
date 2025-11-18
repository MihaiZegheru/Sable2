#include <iostream>
#include <cassert>

#include "core/entity_manager.h"
#include "core/archetype.h"
#include "core/types.h"
#include "core/ecs_manager.h"

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

void TestArchetypeManager() {
    using namespace Core;
    std::cout << "=== Testing ArchetypeManager ===" << std::endl;

    ArchetypeManager& manager = ECSManager::GetInstance().GetArchetypeManager();
    std::cout << "Singleton retrieved successfully." << std::endl;

    // Register attribute types
    manager.RegisterAttributeType(0, sizeof(Position));
    std::cout << "Registered Position attribute." << std::endl;
    manager.RegisterAttributeType(1, sizeof(Velocity));
    std::cout << "Registered Velocity attribute." << std::endl;

    // Create a valid signature
    ArchetypeSignature sig;
    sig.set(0); // Position
    sig.set(1); // Velocity
    std::cout << "Created archetype signature." << std::endl;

    // Add entity
    EntityID entity = 42;
    manager.AddEntity(entity, sig);
    std::cout << "Added entity with ID: " << entity << std::endl;

    // Set and get attribute
    Position pos(1.0f, 2.0f, 3.0f);
    manager.SetAttribute(entity, 0, pos);
    std::cout << "Set Position attribute for entity." << std::endl;
    auto attr = manager.GetAttribute(entity, 0);
    assert(attr.has_value());
    Position& pos_ref = static_cast<Position&>(attr->get());
    assert(pos_ref.x == 1.0f && pos_ref.y == 2.0f && pos_ref.z == 3.0f);
    std::cout << "Verified Position attribute for entity." << std::endl;

    // Update entity archetype
    ArchetypeSignature sig2;
    sig2.set(0); // Only Position
    manager.UpdateEntityArchetype(entity, sig2);
    std::cout << "Updated entity archetype." << std::endl;

    // Remove entity
    manager.RemoveEntity(entity);
    std::cout << "Removed entity." << std::endl;
    auto attr2 = manager.GetAttribute(entity, 0);
    assert(!attr2.has_value());
    std::cout << "Verified entity removal." << std::endl;

    // Error: Remove non-existent entity
    try {
        manager.RemoveEntity(9999);
        std::cerr << "FAILED: No exception for removing non-existent entity!" << std::endl;
        assert(false);
    } catch (const std::runtime_error&) {
        std::cout << "PASSED: Exception thrown for removing non-existent entity." << std::endl;
    }

    std::cout << "=== ArchetypeManager Tests Passed ===" << std::endl << std::endl;
}

void TestECSManager() {
    using namespace Core;
    std::cout << "=== Testing ECSManager ===" << std::endl;

    try {
        // Access ECSManager singleton
        ECSManager& ecs_manager = ECSManager::GetInstance();
        std::cout << "ECSManager singleton retrieved successfully." << std::endl;

        // Test ArchetypeManager retrieval
        ArchetypeManager& archetype_manager = ecs_manager.GetArchetypeManager();
        std::cout << "ArchetypeManager retrieved successfully." << std::endl;

        // Test EntityManager retrieval
        EntityManager& entity_manager = ecs_manager.GetEntityManager();
        std::cout << "EntityManager retrieved successfully." << std::endl;

        // Add additional ECSManager-specific tests here

        std::cout << "=== ECSManager Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ECSManager test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
}

// Define a proper attribute that inherits from IAttribute
struct IntAttribute : Core::IAttribute {
    int value;
    IntAttribute(int v) : value(v) {}
};

void TestECSManagerFunctions() {
    using namespace Core;
    std::cout << "=== Testing ECSManager Functions ===" << std::endl;

    try {
        // Access ECSManager singleton
        ECSManager& ecs_manager = ECSManager::GetInstance();
        std::cout << "ECSManager singleton retrieved successfully." << std::endl;

        // Test RegisterAttribute
        ecs_manager.RegisterAttribute<IntAttribute>();
        std::cout << "Attribute registered successfully." << std::endl;

        // Test AddAttribute
        Entity entity = ecs_manager.CreateEntity();
        std::cout << "Entity created with ID: " << entity.id << std::endl;

        IntAttribute int_attr(42);
        ecs_manager.AddAttribute(entity, int_attr); // Add an IntAttribute
        std::cout << "Attribute added to entity successfully." << std::endl;

        // Test GetAttribute
        IntAttribute& retrieved_attr = ecs_manager.GetAttribute<IntAttribute>(entity);
        assert(retrieved_attr.value == 42);
        std::cout << "Attribute retrieved successfully: " << retrieved_attr.value << std::endl;

        // Test RemoveAttribute
        ecs_manager.RemoveAttribute<IntAttribute>(entity);
        std::cout << "Attribute removed successfully." << std::endl;

        // Test DestroyEntity
        ecs_manager.DestroyEntity(entity);
        std::cout << "Entity destroyed successfully." << std::endl;

        std::cout << "=== All ECSManager Function Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ECSManager function test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
}

// Call the test function in main
int main() {
    try {
        TestECSManagerFunctions();
        TestECSManager();

        // Test EntityManager
        std::cout << "=== Testing EntityManager ===" << std::endl;
        Core::EntityManager& entity_manager = Core::ECSManager::GetInstance().GetEntityManager();
        auto entity = entity_manager.CreateEntity();
        if (!entity) {
            std::cerr << "Failed to create entity!" << std::endl;
            return 1;
        }
        std::cout << "Created entity with ID: " << entity->id << std::endl;
        std::cout << "=== EntityManager Tests Passed! ===" << std::endl << std::endl;

        // Test Archetype Manager
        std::cout << "=== Testing ArchetypeManager ===" << std::endl;
        Core::ArchetypeManager& archetype_manager = Core::ECSManager::GetInstance().GetArchetypeManager();
        TestArchetypeManager();

        // Register the IntAttribute type
        Core::ECSManager::GetInstance().RegisterAttribute<IntAttribute>();

        // Create an entity
        Core::Entity entity2 = Core::ECSManager::GetInstance().CreateEntity();

        // Add an IntAttribute to the entity
        IntAttribute int_attr(42);
        Core::ECSManager::GetInstance().AddAttribute(entity2, int_attr);

        // Retrieve and verify the attribute
        IntAttribute& retrieved_attr = Core::ECSManager::GetInstance().GetAttribute<IntAttribute>(entity2);
        std::cout << "Retrieved attribute value: " << retrieved_attr.value << std::endl;

        // Remove the attribute
        Core::ECSManager::GetInstance().RemoveAttribute<IntAttribute>(entity2);

        // Destroy the entity
        Core::ECSManager::GetInstance().DestroyEntity(entity2);

        std::cout << "All operations completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
