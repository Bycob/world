#include "WorldKeys.h"

#include <sstream>
#include <iomanip>

#include "WorldTypes.h"

namespace world {

std::string ChunkKeys::toString(const world::ChunkKey &key) {
    std::stringstream stream;
    for (char c : key) {
        stream << std::setfill('0') << std::setw(2) << std::hex << (int)c;
    }
    return stream.str();
}

ChunkKey ChunkKeys::fromString(const std::string &str) {
    ChunkKey result(str.length() / 2, 0);

    for (int i = 0; i < result.length(); ++i) {
        result[i] =
            static_cast<char>(std::stoi(str.substr(i * 2, 2), nullptr, 16));
    }

    return result;
}

ObjectKey ObjectKeys::fromString(const std::string &str) {
    return std::stoi(str);
}

AssetKey AssetKeys::fromString(const std::string &str) {
    return std::stoi(str);
}

} // namespace world