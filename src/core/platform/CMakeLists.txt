add_library(platform STATIC
	window.cpp
)

target_include_directories(platform PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(platform PUBLIC
	glm
	glad
	glfw
)
