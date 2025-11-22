add_library(train_managers STATIC
	map_manager.cpp
)

target_include_directories(train_managers PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(train_managers PUBLIC
	ecs
	attributes
	graphics
	systems
	render
	assetloader
)
