#include "WorldKeys.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "WorldTypes.h"

namespace world {

NodeKey NodeKeys::fromUint(unsigned int id) { return std::to_string(id); }

NodeKey NodeKeys::fromInt(int id) { return std::to_string(id); }

std::string NodeKeys::toString(const world::NodeKey &key) {
    std::stringstream stream;
    for (char c : key) {
        auto cint = static_cast<uint32_t>(static_cast<uint8_t>(c));
        stream << std::setfill('0') << std::setw(2) << std::hex << cint;
    }
    return stream.str();
}

NodeKey NodeKeys::fromString(const std::string &str) {
    NodeKey result(str.length() / 2, 0);

    for (size_t i = 0; i < result.length(); ++i) {
        try {
            result[i] =
                static_cast<char>(std::stoi(str.substr(i * 2, 2), nullptr, 16));
        } catch (std::invalid_argument &e) {
            throw e;
        }
    }

    return result;
}

} // namespace world