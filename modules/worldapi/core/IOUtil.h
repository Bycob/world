#pragma once

#include "core/WorldConfig.h"

#include <string>
#include <vector>

namespace world {
bool WORLDAPI_EXPORT createDirectory(const std::string &directory);

void WORLDAPI_EXPORT createDirectories(const std::string &directory);

std::vector<std::string> WORLDAPI_EXPORT
getFileList(const std::string &directory);
} // namespace world
