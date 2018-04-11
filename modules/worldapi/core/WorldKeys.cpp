#include "WorldKeys.h"

#include <sstream>
#include <iomanip>

#include "WorldTypes.h"

namespace world {

    std::string ChunkKeys::toString(const world::ChunkKey &key) {
        std::stringstream stream;
        for (char c : key) {
            stream << std::setfill('0') << std::setw(2) << std::hex << (int) c;
        }
        return stream.str();
    }
}