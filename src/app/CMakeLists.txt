add_executable(sable
	sable.cpp
)

target_link_libraries(sable PRIVATE
	ecs
	render
	systems
	platform
	attributes
	assetloader
)

set_target_properties(sable PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
