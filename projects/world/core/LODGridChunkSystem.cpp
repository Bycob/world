#include "LODGridChunkSystem.h"

#include <string>
#include <map>
#include <vector>

namespace world {

class ChunkEntry {
public:
    template <typename... Args>
    ChunkEntry(const NodeKey &parent, const LODGridCoordinates &coords,
               Args... args)
            : _coords(coords), _chunk(args...), _parentID(parent) {}

    LODGridCoordinates _coords;
    Chunk _chunk;
    NodeKey _parentID;
    std::vector<NodeKey> _children;
};

#define MAX_LOD 20

class LODGridChunkSystemPrivate {
public:
    LODData _lodData[MAX_LOD];
    std::map<NodeKey, std::unique_ptr<ChunkEntry>> _chunks;
    std::vector<std::unique_ptr<IChunkDecorator>> _chunkDecorators;

    // TODO utiliser un tilesystem
};

LODGridChunkSystem::LODGridChunkSystem(double baseChunkSize)
        : _internal(new LODGridChunkSystemPrivate()) {

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
        return std::numeric_limits<double>::epsilon();
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

Chunk &LODGridChunkSystem::getChunk(const vec3d &position, double resolution) {
    int lod = getLODForResolution(resolution);
    vec3d pos = position;
    NodeKey key = NodeKeys::none();

    for (int i = 0; i <= lod; ++i) {
        LODData lodData = getLODData(i);
        vec3d chunkSize = lodData.getChunkSize();
        vec3d tilePos = pos / chunkSize;
        LODGridCoordinates coords{static_cast<s32>(floor(tilePos.x)),
                                  static_cast<s32>(floor(tilePos.y)),
                                  static_cast<s32>(floor(tilePos.z)), i};

        pos -= coords.getPosition3D() * chunkSize;
        key = getChunkKey(key, coords);
    }

    return getChunkByKey(key);
}

void LODGridChunkSystem::collect(ICollector &collector,
                                 const IResolutionModel &resolutionModel) {
    // Explore every toplevel chunk
    auto &lodData = getLODData(0);
    vec3d chunkSize = lodData.getChunkSize();
    BoundingBox bounds = resolutionModel.getBounds();
    vec3d lower = bounds.getLowerBound() / chunkSize;
    vec3d upper = bounds.getUpperBound() / chunkSize;

    for (s32 z = static_cast<s32>(floor(lower.z));
         z <= static_cast<s32>(floor(upper.z)); ++z) {
        for (s32 y = static_cast<s32>(floor(lower.y));
             y <= static_cast<s32>(floor(upper.y)); ++y) {
            for (s32 x = static_cast<s32>(floor(lower.x));
                 x <= static_cast<s32>(floor(upper.x)); ++x) {
                LODGridCoordinates coords{x, y, z, 0};
                NodeKey chunkKey = getChunkKey(NodeKeys::none(), coords);
                collectChunk(chunkKey, collector, resolutionModel);
            }
        }
    }
}

void LODGridChunkSystem::collectChunk(const NodeKey &chunkKey,
                                      world::ICollector &collector,
                                      const IResolutionModel &resolutionModel) {

    LODGridCoordinates coords = dropLastPart(chunkKey);
    LODData &lodData = getLODData(coords.getLOD());
    vec3d chunkSize = lodData.getChunkSize();
    vec3d parentOffset = getOffset(getParentKey(chunkKey));
    BoundingBox bbox{parentOffset + coords.getPosition3D() * chunkSize,
                     parentOffset +
                         (coords.getPosition3D() + vec3i{1, 1, 1}) * chunkSize};
    double resolution = resolutionModel.getMaxResolutionIn(bbox);

    if (resolution > getMinResolution(coords.getLOD())) {
        Chunk &chunk = getChunkByKey(chunkKey);

        // Collect current chunk
        ExplorationContext ctx;
        ctx.appendPrefix(chunkKey);

        chunk.collect(collector, resolutionModel, ctx);

        // Collect the children if they exist
        if (coords.getLOD() < _maxLOD) {
            int lod = coords.getLOD() + 1;

            // Chunk size is divided by _factor every time lod is incremented
            for (u32 x = 0; x < _factor; x++) {
                for (u32 y = 0; y < _factor; y++) {
                    for (u32 z = 0; z < _factor; z++) {
                        LODGridCoordinates ncoords(x, y, z, lod);
                        auto id = getChunkKey(chunkKey, ncoords);
                        collectChunk(chunkKey, collector, resolutionModel);
                    }
                }
            }
        }
    }
}

NodeKey LODGridChunkSystem::getChunkKey(
    const NodeKey &parent, const LODGridCoordinates &coords) const {
    std::stringstream stream;
    stream << parent;
    stream << coords.uid();
    return stream.str();
}

LODGridCoordinates LODGridChunkSystem::dropLastPart(const NodeKey &key) const {
    static const auto uidLength = LODGridCoordinates::NONE.uid().length();
    return LODGridCoordinates::fromUID(key.substr(key.length() - uidLength));
}

NodeKey LODGridChunkSystem::getParentKey(const NodeKey &key) const {
    static const auto uidLength = LODGridCoordinates::NONE.uid().length();
    return key.substr(0, key.length() - uidLength);
}

vec3d LODGridChunkSystem::getOffset(const NodeKey &key) const {
    if (key == NodeKeys::none()) {
        return {};
    } else {
        LODGridCoordinates coords = dropLastPart(key);
        vec3d chunkSize = getLODData(coords.getLOD()).getChunkSize();
        return coords.getPosition3D() * chunkSize +
               getOffset(getParentKey(key));
    }
}

Chunk &LODGridChunkSystem::getChunkByKey(const NodeKey &key) {
    createChunk(key);
    return _internal->_chunks[key]->_chunk;
}

bool LODGridChunkSystem::createChunk(const NodeKey &id) {
    auto chunk = _internal->_chunks.find(id);

    if (chunk == _internal->_chunks.end()) {
        // Get parent properties
        NodeKey parentKey = getParentKey(id);

        if (!parentKey.empty()) {
            createChunk(parentKey);
        }

        auto coords = dropLastPart(id);

        // Get the size of the newly created chunk
        int lod = coords.getLOD();
        LODData &lodData = getLODData(lod);
        auto &chunkSize = lodData.getChunkSize();

        // Create the chunk
        auto &entry = (_internal->_chunks[id] = std::make_unique<ChunkEntry>(
                           parentKey, coords,
                           coords.getPosition3D() * chunkSize, chunkSize));
        entry->_chunk.setResolutionLimits(getMinResolution(lod),
                                          getMaxResolution(lod));

        // Decorate the chunk
        for (auto &chunkDecorator : _internal->_chunkDecorators) {
            chunkDecorator->decorate(entry->_chunk);
        }

        return true;
    } else {
        return false;
    }
}


void LODGridChunkSystem::addDecoratorInternal(IChunkDecorator *decorator) {
    _internal->_chunkDecorators.emplace_back(decorator);
}

} // namespace world
