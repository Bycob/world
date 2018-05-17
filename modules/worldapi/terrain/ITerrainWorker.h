#ifndef WORLD_ITERRAINGENERATOR_H
#define WORLD_ITERRAINGENERATOR_H

#include "core/WorldConfig.h"

#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT ITileContext {
public:
    virtual ~ITileContext() = default;

    virtual Terrain &getTerrain() const = 0;

    virtual Image &getTexture() const = 0;

    /** Returns neighbour terrains. */
    virtual optional<const Terrain &> getNeighbour(int x, int y) const = 0;

    virtual optional<const Terrain &> getParent() const = 0;

    virtual int getParentCount() const = 0;

    virtual vec2i getTileCoords() const = 0;

    /** Tells the context that a version of the processed terrain
     * should be cached right after this generation stage is finished.
     * The cached terrain will then be returned by the method
     * getNeighbour(). */
    virtual void registerCurrentState() = 0;
};

class WORLDAPI_EXPORT ITerrainWorker {
public:
    virtual ~ITerrainWorker() = default;

    virtual void processTerrain(Terrain &terrain) = 0;

    virtual void processTile(ITileContext &context) = 0;
};
} // namespace world

#endif // WORLD_ITERRAINGENERATOR_H
