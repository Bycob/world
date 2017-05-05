#pragma once

#include <worldapi/worldapidef.h>

#include <string>

class WORLDAPI_EXPORT WorldFolder {
public:
	WorldFolder(const std::string & path = "");
	~WorldFolder();

	bool exists() const;
	void setPath(const std::string & path);
private:
	std::string _path;
	bool _exists;

	void init();
};

