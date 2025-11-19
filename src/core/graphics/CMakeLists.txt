add_library(graphics INTERFACE)

target_include_directories(graphics INTERFACE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(graphics INTERFACE
	glad
	glm
)
