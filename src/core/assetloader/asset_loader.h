#ifndef CORE_ASSETLOADER_ASSET_LOADER_H
#define CORE_ASSETLOADER_ASSET_LOADER_H

#include <string>

#include "core/graphics/model.h"

namespace core::assetloader {

graphics::Model ImportModel(const std::string& path);
} // namespace core::assetloader

#endif // CORE_ASSETLOADER_ASSET_LOADER_H
