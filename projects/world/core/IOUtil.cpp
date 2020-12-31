#include "IOUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <sys/stat.h>
#include <regex>
#include <iomanip>

#ifdef _WIN32
#include <direct.h>
#endif

#include <tinydir/tinydir.h>

#include "StringOps.h"

namespace world {

bool createDirectory(const std::string &directory) {
    int error;
#ifdef _WIN32
    error = _mkdir(directory.c_str());
#else
    error = mkdir(directory.c_str(), 0700);
#endif
    if (error != 0) {
        if (errno == EEXIST) {
            return false;
        } else {
            perror("Error creating directory ");
            throw std::runtime_error("The directory " + directory +
                                     " was not created.");
        }
    }

    return true;
}

void createDirectories(const std::string &path) {
    auto directories = split(path, '/');
    std::string currentPath;

    int dirsCreated = 0;

    for (auto &dir : directories) {
        if (dirsCreated == directories.size() - 1 && dir.empty()) {
            continue;
        }

        currentPath += dir + "/";
        createDirectory(currentPath);
        dirsCreated++;
    }
}

std::vector<std::string> getFileList(const std::string &directory) {
    std::vector<std::string> result;

    tinydir_dir dir;
    int error = tinydir_open(&dir, directory.c_str());

    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        if (file.is_dir) {
            result.push_back(std::string(file.name) + "/");
        } else {
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

void removeDirectory(const std::string &directory) {
    std::cerr << "TODO Directory removal not implemented, could not remove " +
                     directory
              << std::endl;
}

#if defined(WIN32)
#else
#include <sys/resource.h>
#endif

long getMemoryUsage() {
#if defined(WIN32)
    return 0;
#else
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
#endif
}

std::string getReadableMemoryUsage(int digits) {
    double memuse = static_cast<double>(getMemoryUsage());
    std::string exps[] = {"kB", "MB", "GB"};

    int i = 0;
    while (i < sizeof(exps) - 1 && memuse > 1000) {
        i++;
        memuse /= 1000;
    }

    std::stringstream sstream;
    sstream << std::setprecision(digits) << memuse << exps[i];
    return sstream.str();
}

} // namespace world