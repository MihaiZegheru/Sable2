add_library(time
	apptime.cpp
)

target_include_directories(time PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(time PUBLIC
)
