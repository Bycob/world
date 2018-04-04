#ifndef WORLD_ITERRAINGENERATOR_H
#define WORLD_ITERRAINGENERATOR_H

#include "core/WorldConfig.h"

#include "Terrain.h"

namespace world {

    class WORLDAPI_EXPORT ITerrainWorkerContext {
    public:
        /** Returns neighbour terrains. */
        virtual optional<const Terrain &> getNeighbour(int x, int y) const = 0;

        /** Tells the context that a version of the processed terrain
         * should be cached right after this generation stage is finished.
         * The cached terrain will then be returned by the method
         * getNeighbour(). */
        virtual void registerCurrentState() = 0;
    };

    class WORLDAPI_EXPORT ITerrainWorker {
    public:
        virtual void process(Terrain& terrain) = 0;

        virtual void process(Terrain& terrain, ITerrainWorkerContext &context) = 0;
    };
}

#endif //WORLD_ITERRAINGENERATOR_H
