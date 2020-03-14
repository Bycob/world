#include "WorldFile.h"

namespace world {

WorldFile::WorldFile() {}

void WorldFile::addString(const std::string &id, const std::string &str) {}

std::string WorldFile::readString(const std::string &id) const {
    return std::__cxx11::string();
}

bool WorldFile::readStringOpt(const std::string &id, std::string &str) const {
    return false;
}

void WorldFile::addFloating(const std::string &id, double f) {}

double WorldFile::readDouble(const std::string &id) const { return 0; }

bool WorldFile::readDoubleOpt(const std::string &id, double &d) const {
    return false;
}

float WorldFile::readFloat(const std::string &id) const { return 0; }

bool WorldFile::readFloatOpt(const std::string &id, float &f) const {
    return false;
}

void WorldFile::addInt(const std::string &id, int i) {}

int WorldFile::readInt(const std::string &id) const { return 0; }

bool WorldFile::readIntOpt(const std::string &id, int &i) const {
    return false;
}

void WorldFile::addBool(const std::string &id, bool b) {}

bool WorldFile::readBool(const std::string &id) const { return false; }

bool WorldFile::readBoolOpt(const std::string &id, bool &b) const {
    return false;
}

void WorldFile::addArray(const std::string &id,
                         const std::vector<WorldFile> &array) {}

void WorldFile::addToArray(const std::string &id, const WorldFile &item) {}

std::vector<WorldFile> WorldFile::readArray(const std::string &id) const {
    return std::vector<WorldFile>();
}

bool WorldFile::readArrayOpt(const std::string &id,
                             std::vector<WorldFile> &array) const {
    return false;
}

void WorldFile::addChild(const std::string &id, const WorldFile &child) {}

WorldFile WorldFile::readChild(const std::string &id) const {
    return WorldFile();
}

void WorldFile::write(const std::string &filename) const {}

void WorldFile::read(const std::string &filename) {}
} // namespace world
