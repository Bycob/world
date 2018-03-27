#ifndef WORLD_FLATWORLDCOLLECTOR_H
#define WORLD_FLATWORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <map>

#include "Collector.h"
#include "FlatWorld.h"
#include "../terrain/Terrain.h"

namespace world {

    class TerrainIterator;

    class WORLDAPI_EXPORT FlatWorldCollector : public Collector {
    public:
        typedef std::string TerrainKey;

        FlatWorldCollector();

        ~FlatWorldCollector() override;

        void reset() override;

        // TODO TerrainStream
        void addTerrain(TerrainKey key, const Terrain &terrain);

        void disableTerrain(TerrainKey key);

        TerrainIterator iterateTerrains();

    private:
        friend class TerrainIterator;

        std::set<TerrainKey> _disabledTerrains;
        std::map<TerrainKey, Terrain> _terrains;
    };

    class PrivateTerrainIterator;

    class WORLDAPI_EXPORT TerrainIterator
            : public std::iterator<std::forward_iterator_tag, std::pair<long, Terrain *>> {
    public:
        TerrainIterator(FlatWorldCollector &collector);

        ~TerrainIterator();

        void operator++();

        std::pair<FlatWorldCollector::TerrainKey, Terrain *> operator*();

        bool hasNext() const;

    private:
        PrivateTerrainIterator *_internal;

        FlatWorldCollector &_collector;
    };

}

#endif //WORLD_FLATWORLDCOLLECTOR_H
