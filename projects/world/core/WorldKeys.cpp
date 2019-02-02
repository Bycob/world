#include "WorldKeys.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "WorldTypes.h"

namespace world {

std::string ChunkKeys::toString(const world::ChunkKey &key) {
    std::stringstream stream;
    for (char c : key) {
        auto cint = static_cast<uint32_t>(static_cast<uint8_t>(c));
        stream << std::setfill('0') << std::setw(2) << std::hex << cint;
    }
    return stream.str();
}

ChunkKey ChunkKeys::fromString(const std::string &str) {
    ChunkKey result(str.length() / 2, 0);

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

ObjectKey ObjectKeys::fromString(const std::string &str) {
    try {
        return std::stoi(str);
    } catch (std::invalid_argument &e) {
        throw e;
    } catch (std::out_of_range &) {
        throw std::invalid_argument("out of range");
    }
}

AssetKey AssetKeys::fromString(const std::string &str) {
    try {
        return std::stoi(str);
    } catch (std::invalid_argument &e) {
        throw e;
    } catch (std::out_of_range &) {
        throw std::invalid_argument("out of range");
    }
}

} // namespace world