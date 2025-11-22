add_library(trains_systems STATIC
	train_system.cpp
)

target_include_directories(trains_systems PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(trains_systems PUBLIC
	glm
	trains_attributes
	ecs
	render
)
