
#ifndef STRING_OPS
#define STRING_OPS

#include "core/WorldConfig.h"

#include <vector>
#include <sstream>

namespace world {

inline std::vector<std::string> split(const std::string &splitted,
                                      const char delim,
                                      bool trimEmpty = false) {
    std::vector<std::string> result;
    std::stringstream ss;
    ss.str(splitted);
    std::string line;

    while (getline(ss, line, delim)) {
        if (trimEmpty && line.empty())
            continue;
        result.push_back(line);
    }

    // at the end geline treats delim as a terminator.
    // if we don't trim empty, we must re-add the empty string manually
    if (!trimEmpty && splitted.back() == delim) {
        result.emplace_back("");
    }
    return result;
}

inline std::string trimSpaces(const std::string &item) {
    size_t startIndex = 0, endIndex = item.size();

    for (size_t i = 0; i < item.size(); i++) {
        if (item[i] == ' ' || item[i] == '\t') {
            ++startIndex;
        } else {
            break;
        }
    }
    for (size_t i = endIndex - 1;; --i) {
        if (item[i] == ' ' || item[i] == '\t') {
            endIndex--;
        } else {
            break;
        }
        if (i == startIndex) {
            break;
        }
    }
    return item.substr(startIndex, endIndex - startIndex);
}

inline bool startsWith(const std::string &item, const std::string &prefix) {
    if (item.size() < prefix.size()) {
        return false;
    }

    auto itemIt = item.begin();
    auto prefixIt = prefix.begin();

    while (prefixIt != prefix.end()) {
        if (*itemIt != *prefixIt)
            return false;

        ++itemIt;
        ++prefixIt;
    }

    return true;
}

inline bool endsWith(const std::string &item, const std::string &suffix) {
    if (item.size() < suffix.size()) {
        return false;
    }

    auto itemIt = item.end();
    auto suffixIt = suffix.end();

    while (suffixIt != suffix.begin()) {
        --itemIt;
        --suffixIt;

        if (*itemIt != *suffixIt)
            return false;
    }

    return true;
}
} // namespace world

#endif // STRING_OPS