#include "RCLib.h"

#include <map>

namespace %namespace% {

    std::map<std::string, std::vector<char>> &getFiles() {
        static std::map<std::string, std::vector<char>> _files{
#include RCLIB_RESOURCES_HEADER
        };
        return _files;
    }

    /*std::string readFileStr(const std::string &filename) {
        auto &files = getFiles();
        return files.at(filename);
    }*/

    std::vector<char> Resources::readFile(const std::string &filename) {
        auto &files = getFiles();
        return files.at(filename);
    }
}
