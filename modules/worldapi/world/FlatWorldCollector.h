#ifndef WORLD_FLATWORLDCOLLECTOR_H
#define WORLD_FLATWORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <map>

#include "Collector.h"
#include "FlatWorld.h"
#include "../terrain/Terrain.h"

namespace world {

    class FlatWorldCollector;

    class PrivateTerrainIterator;

    class WORLDAPI_EXPORT TerrainIterator
            : public std::iterator<std::forward_iterator_tag, std::pair<long, Terrain *>> {
    public:
        TerrainIterator(FlatWorldCollector &collector);

        ~TerrainIterator();

        void operator++();

        std::pair<uint64_t, Terrain *> operator*();

        bool hasNext() const;

    private:
        PrivateTerrainIterator *_internal;

        FlatWorldCollector &_collector;
    };

    class WORLDAPI_EXPORT FlatWorldCollector : public Collector, public ICollector<FlatWorld> {
    public:
        typedef uint64_t TerrainKey;

        FlatWorldCollector();

        ~FlatWorldCollector() override;

        void reset() override;

        void collect(FlatWorld &world, WorldZone &zone) override;

        // TODO TerrainStream
        void addTerrain(TerrainKey key, const Terrain &terrain);

        void disableTerrain(TerrainKey key);

        TerrainIterator iterateTerrains();

    private:
        friend class TerrainIterator;

        std::set<TerrainKey> _disabledTerrains;
        std::map<TerrainKey, Terrain> _terrains;
    };

}

#endif //WORLD_FLATWORLDCOLLECTOR_H
