add_library(render STATIC
	renderer.cpp
)

target_include_directories(render PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(render PUBLIC
	glm
	glad
	graphics
)
