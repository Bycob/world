#include "WorldFolder.h"

#include <algorithm>

#include <tinydir/tinydir.h>

#include "IOUtil.h"

WorldFolder::WorldFolder(const std::string & path) 
	: _path(path), _filelist() {

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

const std::vector<std::string> & WorldFolder::getFileList() const {
	return _filelist;
}

std::ofstream WorldFolder::getFile(const std::string & name) {
	std::ofstream result(_path + name);

	return result;
}

bool WorldFolder::hasFile(const std::string & name) const {
	return std::find(_filelist.begin(), _filelist.end(), name) != _filelist.end();
}

void WorldFolder::refresh() {
	init();
}

void WorldFolder::init() {
	_exists = false;
	if (_path == "") return;

	_filelist = ioutil::getFileList(_path);
}
