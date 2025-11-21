add_library(managers2 STATIC
	map_manager.cpp
)

target_include_directories(managers2 PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(managers2 PUBLIC
	ecs
	attributes
	graphics
	systems
	render
	assetloader
)
