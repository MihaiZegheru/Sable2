cmake_minimum_required(VERSION 3.10)
project(Sable)

set(CMAKE_CXX_STANDARD 20)

# Define PROJECT_SOURCE_DIR and ABSOLUTE_RESOURCE_DIR for use in the code
file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/src" PROJECT_SOURCE_DIR)
add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/res" ABSOLUTE_RESOURCE_DIR)
add_compile_definitions(ABSOLUTE_RESOURCE_DIR="${ABSOLUTE_RESOURCE_DIR}")

# Third-party libraries
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/lib/glad/include)

add_library(stb_image STATIC ${CMAKE_SOURCE_DIR}/lib/stb-image/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb-image)

# Subdirectories
add_subdirectory("lib/glfw-3.3.4")
add_subdirectory("lib/assimp-5.0.1")
add_subdirectory("lib/glm-0.9.9.8")
add_subdirectory(src)
