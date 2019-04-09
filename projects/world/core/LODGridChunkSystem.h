#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include "world/core/WorldConfig.h"

#include "WorldTypes.h"
#include "WorldNode.h"
#include "IChunkSystem.h"
#include "Chunk.h"
#include "LODGridCoordinates.h"
#include "LODData.h"
#include "IChunkDecorator.h"

namespace world {

class LODGridChunkSystemPrivate;
class ChunkEntry;

class WORLDAPI_EXPORT LODGridChunkSystem : public WorldNode, IChunkSystem {
public:
    LODGridChunkSystem(double baseChunkSize = 1000);

    ~LODGridChunkSystem();

    LODData &getLODData(int lod) const;

    /** Compute the offset of the chunk corresponding to this key, in
     * world unit (meters). */
    vec3d getOffset(const NodeKey &key) const;

    /** Gives the maximum resolution the given LOD can hold. All
     * objects with a better resolution should be stored in a
     * higher LOD.
     * The resolution is given in points per meter. For exemple, a mesh
     * whose faces are 20 cm large has a resolution of 5-6. */
    double getMaxResolution(int lod) const;

    /** Gives the minimum resolution required for an object to be
     * contained in the given LOD. All objects with a worse resolution
     * should be stored in a lower LOD.
     * The resolution is given in points per meter. For exemple, a mesh
     * whose faces are 20 cm large has a resolution of 5-6. */
    double getMinResolution(int lod) const;

    int getLODForResolution(double mrd) const;

    Chunk &getChunk(const vec3d &position, double resolution) override;

    void collect(ICollector &collector,
                 const IResolutionModel &resolutionModel,
                 const ExplorationContext &ctx = ExplorationContext::getDefault()) override;

    template <typename T, typename... Args> T &addDecorator(Args&... args);

protected:
    /** Test if the given chunk should be collected. If it is the case,
     * then the chunk is collected and collectChunk is called on each
     * of its children. */
    void collectChunk(const NodeKey &chunkKey, ICollector &collector,
                      const IResolutionModel &resolutionModel);

private:
    LODGridChunkSystemPrivate *_internal;

    /** If an object has a greater resolution than this value,
     * we can't put it in the minimum LOD. */
    double _subdivResolutionThreshold = 0.5;

    u32 _factor = 2;

    int _maxLOD = 6;

    
    /** This method returns the chunk entry corresponding to this key. If the
     * chunk does not exist it is created. */
    ChunkEntry &getOrCreateEntry(const NodeKey &key);

    void addDecoratorInternal(IChunkDecorator *decorator);
};


template <typename T, typename... Args>
T &LODGridChunkSystem::addDecorator(Args&... args) {
    T *decorator = new T(args...);
    addDecoratorInternal(decorator);
    return *decorator;
}

} // namespace world

#endif // WORLD_CHUNKSYSTEM_H
