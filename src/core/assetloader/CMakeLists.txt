add_library(assetloader STATIC
	asset_loader_manager.cpp
)

target_include_directories(assetloader PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(assetloader PUBLIC
	assimp
	graphics
	stb_image
)
