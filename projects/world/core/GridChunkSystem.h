#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include "world/core/WorldConfig.h"

#include "WorldTypes.h"
#include "WorldNode.h"
#include "IChunkSystem.h"
#include "Chunk.h"
#include "IChunkDecorator.h"
#include "TileSystem.h"

namespace world {

class GridChunkSystemPrivate;
class ChunkEntry;

class WORLDAPI_EXPORT GridChunkSystem : public WorldNode, IChunkSystem {
public:
    /**
     * @param baseChunkSize Size of the highest level of details
     * @param maxLod maximum allowed level of details
     * @param baseRes maximum resolution of lod #0
     */
    GridChunkSystem(double baseChunkSize = 1000, int maxLod = 6,
                    double baseRes = 0.5);

    ~GridChunkSystem();

    const TileSystem &getTileSystem() const { return tileSystem(); }

    /** Compute the offset of the chunk corresponding to this key. */
    vec3d getOffset(const TileCoordinates &tc) const;

    Chunk &getChunk(const vec3d &position, double resolution) override;

    void collect(ICollector &collector, const IResolutionModel &resolutionModel,
                 const ExplorationContext &ctx =
                     ExplorationContext::getDefault()) override;

    template <typename T, typename... Args> T &addDecorator(Args &... args);

protected:
    /** Test if the given chunk should be collected. If it is the case,
     * then the chunk is collected and collectChunk is called on each
     * of its children. */
    void collectChunk(const TileCoordinates &chunkKey, ICollector &collector,
                      const IResolutionModel &resolutionModel,
                      const ExplorationContext &ctx);

private:
    GridChunkSystemPrivate *_internal;


    ChunkEntry &getOrCreateEntry(const TileCoordinates &tc);

    void addDecoratorInternal(IChunkDecorator *decorator);

    TileSystem &tileSystem() const;

    friend class ChunkEntry;
};


template <typename T, typename... Args>
T &GridChunkSystem::addDecorator(Args &... args) {
    T *decorator = new T(args...);
    addDecoratorInternal(decorator);
    return *decorator;
}

} // namespace world

#endif // WORLD_CHUNKSYSTEM_H
