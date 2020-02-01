#include "LODGridChunkSystem.h"

#include <string>
#include <map>
#include <vector>

#include "TileSystem.h"
#include "GridStorage.h"

namespace world {

class ChunkEntry : public IGridElement {
public:
    ChunkEntry(const TileCoordinates &tc, const TileSystem &ts,
               LODGridChunkSystem &chunkSystem);

    TileCoordinates _coords;
    Chunk _chunk;
};

#define MAX_LOD 20

class LODGridChunkSystemPrivate {
public:
    LODData _lodData[MAX_LOD];
    std::map<NodeKey, std::unique_ptr<ChunkEntry>> _chunks;
    std::vector<std::unique_ptr<IChunkDecorator>> _chunkDecorators;

    TileSystem _tileSystem;
    GridStorage<ChunkEntry> _storage;

    LODGridChunkSystemPrivate() : _tileSystem(0, {}, {}) {}
};

ChunkEntry::ChunkEntry(const TileCoordinates &tc, const TileSystem &ts,
                       LODGridChunkSystem &chunkSystem)
        : _coords(tc),
          _chunk(ts.getTileSize(tc._lod), ts.getMinResolution(tc._lod),
                 ts.getMaxResolution(tc._lod)) {

    _chunk.setPosition3D(chunkSystem.getOffset(tc));

    for (auto &chunkDecorator : chunkSystem._internal->_chunkDecorators) {
        chunkDecorator->decorate(_chunk);
    }
}


LODGridChunkSystem::LODGridChunkSystem(double baseChunkSize, int maxLod,
                                       double baseRes)
        : _internal(new LODGridChunkSystemPrivate()) {

    TileSystem &ts = tileSystem();
    ts._maxLod = maxLod;
    ts._baseSize = {baseChunkSize};
    // Compute buffer resolution so that the min resolution of lod 1 is equal to
    // baseRes.
    double bufferRes = std::floor(baseRes * baseChunkSize / ts._factor);

    if (bufferRes < 1) {
        throw std::runtime_error(
            "base chunk size " + std::to_string(baseChunkSize) +
            " * base resolution " + std::to_string(baseRes) +
            " are not allowed: less than 1 point per tile.");
    }

    ts._bufferRes = {static_cast<int>(bufferRes)};
}

LODGridChunkSystem::~LODGridChunkSystem() { delete _internal; }

Chunk &LODGridChunkSystem::getChunk(const vec3d &position, double resolution) {
    TileSystem &ts = tileSystem();
    TileCoordinates tc = ts.getTileCoordinates(position, ts.getLod(resolution));
    auto &entry = getOrCreateEntry(tc);
    return entry._chunk;
}

void LODGridChunkSystem::collect(ICollector &collector,
                                 const IResolutionModel &resolutionModel,
                                 const ExplorationContext &ctx) {
    // Run collect on every decorator if needed
    int decoratorID = 0;
    for (auto &decorator : _internal->_chunkDecorators) {
        WorldNode *node = dynamic_cast<WorldNode *>(decorator.get());

        if (node != nullptr) {
            ExplorationContext childCtx = ctx;
            childCtx.appendPrefix({"deco" + std::to_string(decoratorID)});
            node->collect(collector, resolutionModel, childCtx);
        }
        ++decoratorID;
    }

    // Explore chunks
    TileSystem &ts = tileSystem();
    auto &storage = _internal->_storage;
    auto it = ts.iterate(resolutionModel, resolutionModel.getBounds(ctx), true);

    int i = 0;
    for (; !it.endReached(); ++it) {
        TileCoordinates tc = *it;
        collectChunk(tc, collector, resolutionModel, ctx);
    }
}

void LODGridChunkSystem::collectChunk(const TileCoordinates &chunkKey,
                                      ICollector &collector,
                                      const IResolutionModel &resolutionModel,
                                      const ExplorationContext &ctx) {

    Chunk &chunk = getOrCreateEntry(chunkKey)._chunk;
    collectChild(chunkKey.toKey(), chunk, collector, resolutionModel, ctx);
}

vec3d LODGridChunkSystem::getOffset(const TileCoordinates &tc) const {
    return tileSystem().getTileOffset(tc);
}

ChunkEntry &LODGridChunkSystem::getOrCreateEntry(const TileCoordinates &tc) {
    return _internal->_storage.getOrCreate(tc, tc, tileSystem(), *this);
}

void LODGridChunkSystem::addDecoratorInternal(IChunkDecorator *decorator) {
    _internal->_chunkDecorators.emplace_back(decorator);
}

TileSystem &LODGridChunkSystem::tileSystem() const {
    return _internal->_tileSystem;
}

} // namespace world
