#pragma once

#include <worldapi/worldapidef.h>

#include <string>
#include <vector>

namespace ioutil {
	void WORLDAPI_EXPORT createDirectory(const std::string &directory);
	std::vector<std::string> WORLDAPI_EXPORT getFileList(const std::string &directory);
}
