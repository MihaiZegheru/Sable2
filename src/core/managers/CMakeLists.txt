add_library(managers STATIC
	hierarchy_manager.cpp
	scene_manager.cpp
)

target_include_directories(managers PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(managers PUBLIC

)
