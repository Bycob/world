#include "WorldFolder.h"


WorldFolder::WorldFolder(const std::string & path) 
	: _path(path) {

	init();
}

WorldFolder::~WorldFolder() {

}

void WorldFolder::setPath(const std::string & path) {
	_path = path;
	init();
}

bool WorldFolder::exists() const {
	return _exists;
}

void WorldFolder::init() {

}