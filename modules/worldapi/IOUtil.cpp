#include "IOUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include <tinydir/tinydir.h>

namespace ioutil {
	void createDirectory(const std::string &directory) {
		int error;
#ifdef _WIN32
		error = _mkdir(directory.c_str());
#else
		error = mkdir(directory.c_str(), 0700);
#endif
		if (error != 0) {
			if (errno == EEXIST) {}
			else {
				throw std::runtime_error("the directory " + directory + " was not created\nError code : " + std::to_string(errno));
			}
		}
	}

	std::vector<std::string> getFileList(const std::string &directory) {
		std::vector <std::string> result;

		tinydir_dir dir;
		int error = tinydir_open(&dir, directory.c_str());

		while (dir.has_next) {
			tinydir_file file;
			tinydir_readfile(&dir, &file);

			if (file.is_dir) {
				result.push_back(std::string(file.name) + "/");
			}
			else {
				result.emplace_back(file.name);
			}

			tinydir_next(&dir);
		}

		tinydir_close(&dir);
		
		if (error != 0) {
			throw std::runtime_error("file not found");
		}

		return result;
	}
}