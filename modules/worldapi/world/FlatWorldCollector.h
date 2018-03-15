#ifndef WORLD_FLATWORLDCOLLECTOR_H
#define WORLD_FLATWORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <map>

#include "WorldCollector.h"
#include "FlatWorld.h"
#include "../terrain/Terrain.h"

class FlatWorldCollector;

class PrivateTerrainIterator;

class WORLDAPI_EXPORT TerrainIterator
        : public std::iterator<std::forward_iterator_tag, std::pair<long, Terrain*>> {
public:
    TerrainIterator(FlatWorldCollector &collector);
    ~TerrainIterator();

    void operator++();
    std::pair<long, Terrain*> operator*();
    bool hasNext() const;
private:
    PrivateTerrainIterator* _internal;

    FlatWorldCollector &_collector;
};

class WORLDAPI_EXPORT FlatWorldCollector : public WorldCollector, public IWorldCollector<FlatWorld> {
public:
    FlatWorldCollector();
    ~FlatWorldCollector() override;

    void reset() override;
    void collect(FlatWorld &world, WorldZone &zone) override;

    // TODO TerrainStream
    void addTerrain(long key, const Terrain &terrain);
    void disableTerrain(long key);
    TerrainIterator iterateTerrains();
private:
    friend class TerrainIterator;

    std::set<long> _disabledTerrains;
    std::map<long, Terrain> _terrains;
};


#endif //WORLD_FLATWORLDCOLLECTOR_H
