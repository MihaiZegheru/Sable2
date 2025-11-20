add_library(attributes INTERFACE)

target_include_directories(attributes INTERFACE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(attributes INTERFACE
	glm
)
