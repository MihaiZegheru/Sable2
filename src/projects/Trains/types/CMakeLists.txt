add_library(trains_types INTERFACE)

target_include_directories(trains_types INTERFACE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(trains_types INTERFACE
)
