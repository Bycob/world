#pragma once
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

namespace ioutil {
	inline void createDirectory(std::string directory) {
#ifdef _WIN32
		mkdir(directory.c_str());
#else
		mkdir(directory.c_str(), 0700);
#endif
	}
}
