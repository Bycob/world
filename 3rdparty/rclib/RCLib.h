#ifndef RCLIB_H
#define RCLIB_H

#include <string>
#include <vector>

namespace %namespace% {
    struct Resources {
        static std::vector<char> readFile(const std::string &filename);
    };
}

#endif //RCLIB_H
