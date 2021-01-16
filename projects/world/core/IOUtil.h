#pragma once

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>

namespace world {
bool WORLDAPI_EXPORT createDirectory(const std::string &directory);

void WORLDAPI_EXPORT createDirectories(const std::string &directory);

std::vector<std::string> WORLDAPI_EXPORT
getFileList(const std::string &directory);

/** Removes a directory recursively. */
void WORLDAPI_EXPORT removeDirectory(const std::string &directory);

bool WORLDAPI_EXPORT fileExists(const std::string &filename);

// TODO Move that on the correct file
/** On linux returns memory usage in kB. */
long WORLDAPI_EXPORT getMemoryUsage();

/** Returns a fancy string containing memory usage with suffix (kB, MB, GB, ...)
 */
std::string WORLDAPI_EXPORT getReadableMemoryUsage(int digits = 3);

} // namespace world
