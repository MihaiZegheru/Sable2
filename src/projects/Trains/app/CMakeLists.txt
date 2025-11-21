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
	managers2
)

set_target_properties(trains PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/trains
)
