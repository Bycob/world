#pragma once

#include "core/WorldConfig.h"

#include <fstream>
#include <vector>
#include <string>

namespace world {

	class WORLDAPI_EXPORT WorldFolder {
	public:
		WorldFolder(const std::string &path = "");

		~WorldFolder();

		bool exists() const;

		void setPath(const std::string &path);

		const std::vector<std::string> &getFileList() const;

		std::ofstream getFile(const std::string &name);

		bool hasFile(const std::string &name) const;

		void refresh();

	private:
		std::string _path;
		bool _exists;
		std::vector<std::string> _filelist;

		void init();
	};
}

