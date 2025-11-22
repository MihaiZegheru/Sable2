add_library(trains_attributes INTERFACE)

target_include_directories(trains_attributes INTERFACE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(trains_attributes INTERFACE
)
