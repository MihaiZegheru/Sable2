add_library(systems STATIC
	camera_system.cpp
)

target_include_directories(systems PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(systems PUBLIC
	glm
	attributes
)
