#pragma once

#include "core/WorldConfig.h"

#include <string>
#include <vector>

namespace world {
	void WORLDAPI_EXPORT createDirectory(const std::string &directory);

	std::vector<std::string> WORLDAPI_EXPORT getFileList(const std::string &directory);
}
