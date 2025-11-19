add_executable(sable
	sable.cpp
)

target_link_libraries(sable PRIVATE
	ecs
)

set_target_properties(sable PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
