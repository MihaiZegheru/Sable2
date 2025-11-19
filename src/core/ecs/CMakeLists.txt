add_library(ecs STATIC
	archetype.cpp
	archetype_manager.cpp
	entity_manager.cpp
	ecs_manager.cpp
	system_manager.cpp
)

target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(ecs PUBLIC
)
