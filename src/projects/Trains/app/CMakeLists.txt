add_executable(trains
	trains.cpp
)

target_link_libraries(trains PRIVATE
	ecs
	render
	systems
	platform
	attributes
	assetloader
	time
	train_managers
	trains_systems
	trains_attributes
)

set_target_properties(trains PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/trains
)
