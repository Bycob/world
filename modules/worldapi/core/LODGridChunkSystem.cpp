#include "LODGridChunkSystem.h"

#include <string>
#include <map>
#include <vector>

namespace world {

class ChunkEntry {
public:
    template <typename... Args>
    ChunkEntry(const ChunkKey &parent, const LODGridCoordinates &coords,
               Args... args)
            : _parentID(parent), _coords(coords), _chunk(args...) {}

    LODGridCoordinates _coords;
    Chunk _chunk;
    ChunkKey _parentID;
    std::vector<ChunkKey> _children;
};

#define MAX_LOD 20

class PLODGridChunkSystem {
public:
    LODData _lodData[MAX_LOD];
    std::map<ChunkKey, std::unique_ptr<ChunkEntry>> _chunks;
};

LODGridChunkSystem::LODGridChunkSystem(double baseChunkSize)
        : _internal(new PLODGridChunkSystem()) {

    // Initialize LODData
    _internal->_lodData[0] =
        LODData({baseChunkSize, baseChunkSize, baseChunkSize},
                _subdivResolutionThreshold);

    for (int i = 0; i <= _maxLOD; i++) {
        if (i != 0) {
            auto dimensions = _internal->_lodData[0].getChunkSize() *
                              powi((double)_factor, -i);
            auto maxResolution = _internal->_lodData[0].getMaxResolution() *
                                 powi((double)_factor, i);
            _internal->_lodData[i] = LODData(dimensions, maxResolution);
        }
        std::cout << getLODData(i).getMaxResolution() << std::endl;
    }
}

LODGridChunkSystem::~LODGridChunkSystem() { delete _internal; }

LODData &LODGridChunkSystem::getLODData(int lod) const {
    if (lod < 0 || lod > _maxLOD) {
        throw std::runtime_error("getLODData : bad index");
    }
    return _internal->_lodData[lod];
}

double LODGridChunkSystem::getMaxResolution(int lod) const {
    if (lod == _maxLOD) {
        return 1e100;
    }

    auto &data = getLODData(lod);
    return data.getMaxResolution();
}

double LODGridChunkSystem::getMinResolution(int lod) const {
    if (lod == 0) {
        return 0;
    }

    auto &data = getLODData(lod - 1);
    return data.getMaxResolution();
}

int LODGridChunkSystem::getLODForResolution(double mrd) const {
    int lod = 0;

    for (; lod < _maxLOD; lod++) {
        double min = getMinResolution(lod);
        double max = getMaxResolution(lod);

        if (mrd >= min && mrd < max) {
            break;
        } else if (mrd >= max && lod == 0) {
            break;
        }
    }

    return lod;
}

WorldZone LODGridChunkSystem::getZone(const vec3d &position) {
    LODData &data = _internal->_lodData[0];
    vec3d intPos = position / data.getChunkSize();

    LODGridCoordinates coords = LODGridCoordinates(
        (int)floor(intPos.x), (int)floor(intPos.y), (int)floor(intPos.z), 0);

    return getZone(getChunkKey(ChunkKeys::none(), coords));
}

std::vector<WorldZone> LODGridChunkSystem::getNeighbourZones(
    const WorldZone &chunk) {
    auto key = chunk->getID();
    LODGridCoordinates coords = dropLastPart(key);
    ChunkKey parentKey = getParentKey(key);

    vec3i directions[] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                          {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};

    std::vector<WorldZone> result;

    for (vec3i &direction : directions) {
        LODGridCoordinates ncoords = LODGridCoordinates(
            coords.getPosition3D() + direction, coords.getLOD());

        auto id = getChunkKey(parentKey, ncoords);
        result.emplace_back(getZone(id));
    }
    return result;
}

std::vector<WorldZone> LODGridChunkSystem::getChildrenZones(
    const WorldZone &zone) {
    std::vector<WorldZone> vector;
    vector.reserve(_factor * _factor * _factor);

    auto key = zone->getID();
    LODGridCoordinates coords = dropLastPart(key);

    // Create children
    if (coords.getLOD() < _maxLOD) {
        int lod = coords.getLOD() + 1;

        // Actuellement, a chaque fois qu'on monte d'un lod, on divise la taille
        // du chunk par 2
        for (int x = 0; x < _factor; x++) {
            for (int y = 0; y < _factor; y++) {
                for (int z = 0; z < _factor; z++) {
                    LODGridCoordinates ncoords(x, y, z, lod);
                    auto id = getChunkKey(key, ncoords);
                    vector.emplace_back(getZone(id));
                }
            }
        }
    }

    return vector;
}

Chunk &LODGridChunkSystem::getChunk(const WorldZone &zone) {
    return getChunk(zone->getID());
}

ChunkKey LODGridChunkSystem::getChunkKey(
    const ChunkKey &parent, const LODGridCoordinates &coords) const {
    std::stringstream stream;
    stream << parent;
    stream << coords.uid();
    return stream.str();
}

LODGridCoordinates LODGridChunkSystem::dropLastPart(const ChunkKey &key) const {
    static const auto uidLength = LODGridCoordinates::NONE.uid().length();
    return LODGridCoordinates::fromUID(key.substr(key.length() - uidLength));
}

ChunkKey LODGridChunkSystem::getParentKey(const ChunkKey &key) const {
    static const auto uidLength = LODGridCoordinates::NONE.uid().length();
    return key.substr(0, key.length() - uidLength);
}

Chunk &LODGridChunkSystem::getChunk(const ChunkKey &id) {
    createChunk(id);
    return _internal->_chunks[id]->_chunk;
}

WorldZone LODGridChunkSystem::getZone(const ChunkKey &id) {
    return WorldZone(id, new LODGridChunkHandler(*this, id));
}

bool LODGridChunkSystem::createChunk(const ChunkKey &id) {

    auto chunk = _internal->_chunks.find(id);

    if (chunk == _internal->_chunks.end()) {
        // Get parent properties
        ChunkKey parentKey = getParentKey(id);

        if (!parentKey.empty()) {
            createChunk(parentKey);
        }

        auto coords = dropLastPart(id);

        // Get the size of the newly created chunk
        int lod = coords.getLOD();
        LODData &lodData = getLODData(lod);
        auto &chunkSize = lodData.getChunkSize();

        // Create the chunk
        auto entry = std::make_unique<ChunkEntry>(
            parentKey, coords, coords.getPosition3D() * chunkSize, chunkSize);
        entry->_chunk.setResolutionLimits(getMinResolution(lod),
                                          getMaxResolution(lod));

        // Add it to the map
        _internal->_chunks[id] = std::move(entry);
        return true;
    } else {
        return false;
    }
}


LODGridChunkHandler::LODGridChunkHandler(LODGridChunkSystem &system,
                                         const std::string &id)
        : _system(system), _id(id), _coordinates(system.dropLastPart(id)) {}

LODGridChunkHandler::LODGridChunkHandler(const LODGridChunkHandler &other)
        : _system(other._system), _id(other._id),
          _coordinates(other._coordinates) {}

ChunkKey LODGridChunkHandler::getID() const { return _id; }

optional<WorldZone> LODGridChunkHandler::getParent() const {
    auto parentKey = _system.getParentKey(_id);
    if (parentKey.empty()) {
        return nullopt;
    } else {
        return _system.getZone(parentKey);
    }
}

vec3d LODGridChunkHandler::getParentOffset() const {
    LODData lodData = _system.getLODData(_coordinates.getLOD());
    return _coordinates.getPosition3D() * getDimensions();
}

double LODGridChunkHandler::getMaxResolution() const {
    return _system.getMaxResolution(_coordinates.getLOD());
}

double LODGridChunkHandler::getMinResolution() const {
    return _system.getMinResolution(_coordinates.getLOD());
}

vec3d LODGridChunkHandler::getDimensions() const {
    return _system.getLODData(_coordinates.getLOD()).getChunkSize();
}
} // namespace world
