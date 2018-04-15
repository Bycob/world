#include "LODGridChunkSystem.h"

#include <string>
#include <map>
#include <vector>

namespace world {

using QueryResult = IChunkSystem::QueryResult;

class ChunkEntry {
public:
    template <typename... Args>
    ChunkEntry(ChunkKey parent, const LODGridCoordinates &coords, Args... args)
            : _parentID(parent), _coords(coords), _chunk(args...) {}

    LODGridCoordinates _coords;
    Chunk _chunk;
    ChunkKey _parentID;
    std::vector<ChunkKey> _children;
};

class PLODGridChunkSystem {
public:
    std::map<int, LODData> _lodData;
    std::map<ChunkKey, std::unique_ptr<ChunkEntry>> _chunks;
};

LODGridChunkSystem::LODGridChunkSystem(double baseChunkSize)
        : _internal(new PLODGridChunkSystem()) {
    _internal->_lodData.emplace(
        0, LODData({baseChunkSize, baseChunkSize, baseChunkSize},
                   _subdivResolutionThreshold));

    for (int i = 0; i <= _maxLOD; i++) {
        std::cout << getLODData(i).getMaxResolution() << std::endl;
    }
}

LODGridChunkSystem::~LODGridChunkSystem() { delete _internal; }

LODData &LODGridChunkSystem::getLODData(int lod) const {
    if (lod < 0 || lod > _maxLOD) {
        throw std::runtime_error("getLODData : bad index");
    }

    auto lodData = _internal->_lodData.find(lod);

    if (lodData == _internal->_lodData.end()) {
        auto dimensions =
            _internal->_lodData[0].getChunkSize() * powi((double)_factor, -lod);
        auto maxResolution = _internal->_lodData[0].getMaxResolution() *
                             powi((double)_factor, lod);
        return _internal->_lodData[lod] = LODData(dimensions, maxResolution);
    } else {
        return (*lodData).second;
    }
}

double LODGridChunkSystem::getMaxResolution(int lod) const {
    if (lod == _maxLOD) {
        return 1e100;
    }

    auto data = getLODData(lod);
    return data.getMaxResolution();
}

double LODGridChunkSystem::getMinResolution(int lod) const {
    if (lod == 0) {
        return 0;
    }

    auto data = getLODData(lod - 1);
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

QueryResult LODGridChunkSystem::getChunk(const vec3d &position) {
    LODData &data = _internal->_lodData[0];
    vec3d intPos = position / data.getChunkSize();

    LODGridCoordinates coords = LODGridCoordinates(
        (int)floor(intPos.x), (int)floor(intPos.y), (int)floor(intPos.z), 0);

    auto id = createChunk(ChunkKeys::none(), coords);
    return {*getZone(id.first), id.second};
}

QueryResult LODGridChunkSystem::getNeighbourChunk(const WorldZone &chunk,
                                                  const vec3i &direction) {
    // TODO avoid creating the same chunk twice (if we call getOrCreateNeighbour
    // wrongly)
    ChunkEntry &entry = *_internal->_chunks[chunk->getID()];
    LODGridCoordinates coords = entry._coords;
    LODGridCoordinates ncoords =
        LODGridCoordinates(coords.getPosition3D() + direction, coords.getLOD());

    auto id = createChunk(entry._parentID, ncoords);
    return {*getZone(id.first), id.second};
}

std::vector<QueryResult> LODGridChunkSystem::getChildren(
    const WorldZone &zone) {
    std::vector<QueryResult> vector;

    ChunkEntry &entry = *_internal->_chunks[zone->getID()];
    bool created = false;

    // Create children
    if (entry._children.empty() && entry._coords.getLOD() < _maxLOD) {
        int lod = entry._coords.getLOD() + 1;

        // Actuellement, a chaque fois qu'on monte d'un lod, on divise la taille
        // du chunk par 2
        for (int x = 0; x < _factor; x++) {
            for (int y = 0; y < _factor; y++) {
                for (int z = 0; z < _factor; z++) {
                    LODGridCoordinates ncoords(x, y, z, lod);
                    auto id = createChunk(zone->getID(), ncoords);
                    entry._children.push_back(id.first);
                }
            }
        }

        created = true;
    }

    std::transform(entry._children.begin(), entry._children.end(),
                   std::inserter(vector, vector.end()),
                   [&](const ChunkKey &id) {

                       return QueryResult{*getZone(id), created};
                   });

    return vector;
}

ChunkKey LODGridChunkSystem::getChunkKey(
    const ChunkKey &parent, const LODGridCoordinates &coords) const {
    std::stringstream stream;
    stream << parent;
    stream << coords.uid();
    return stream.str();
}

// TODO optional ?
Chunk &LODGridChunkSystem::getChunk(const ChunkKey &id) {
    return _internal->_chunks[id]->_chunk;
}

optional<WorldZone> LODGridChunkSystem::getZone(const ChunkKey &id) {
    auto it = _internal->_chunks.find(id);

    if (it == _internal->_chunks.end()) {
        return nullopt;
    } else {
        return WorldZone(new LODGridChunkHandler(*this, id, getChunk(id)));
    }
}

std::pair<ChunkKey, bool> LODGridChunkSystem::createChunk(
    const ChunkKey &parent, const LODGridCoordinates &coords) {
    auto id = getChunkKey(parent, coords);
    auto chunk = _internal->_chunks.find(id);

    if (chunk == _internal->_chunks.end()) {
        // Get the size of the newly created chunk
        int lod = coords.getLOD();
        LODData &lodData = getLODData(lod);
        auto &chunkSize = lodData.getChunkSize();

        // Create the chunk
        auto entry = std::make_unique<ChunkEntry>(
            parent, coords, coords.getPosition3D() * chunkSize, chunkSize);
        entry->_chunk.setResolutionLimits(getMinResolution(lod),
                                          getMaxResolution(lod));

        // Add it to the map
        _internal->_chunks[id] = std::move(entry);
        return {id, true};
    } else {
        return {id, false};
    }
}


LODGridChunkHandler::LODGridChunkHandler(LODGridChunkSystem &system,
                                         const std::string &id, Chunk &chunk)
        : _system(system), _id(id), _chunk(chunk) {}

LODGridChunkHandler::LODGridChunkHandler(const LODGridChunkHandler &other)
        : _system(other._system), _id(other._id), _chunk(other._chunk) {}

IWorldZoneHandler *LODGridChunkHandler::clone() const {
    return new LODGridChunkHandler(*this);
}

Chunk &LODGridChunkHandler::chunk() { return _chunk; }

const Chunk &LODGridChunkHandler::getChunk() const { return _chunk; }

const ChunkKey &LODGridChunkHandler::getID() const { return _id; }

bool LODGridChunkHandler::operator<(const LODGridChunkHandler &other) const {
    return _id < other._id;
}

bool LODGridChunkHandler::operator==(const LODGridChunkHandler &other) const {
    return _id == other._id;
}

bool LODGridChunkHandler::hasParent() const {
    auto &entry = _system._internal->_chunks[_id];
    return entry->_parentID != ChunkKeys::none();
}

optional<WorldZone> LODGridChunkHandler::getParent() const {
    auto &entry = _system._internal->_chunks[_id];
    return _system.getZone(entry->_parentID);
}
} // namespace world
