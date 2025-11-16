add_library(core STATIC
	archetype.cpp
	entity_manager.cpp
)

target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(core PUBLIC
)
