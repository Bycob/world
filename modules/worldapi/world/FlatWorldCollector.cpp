#include "FlatWorldCollector.h"

class PrivateTerrainIterator {
public:
    std::map<long, Terrain>::iterator _it;
};

TerrainIterator::TerrainIterator(FlatWorldCollector &collector)
        : _internal(new PrivateTerrainIterator()),
          _collector(collector) {

    _internal->_it = _collector._terrains.begin();
}

TerrainIterator::~TerrainIterator() {
    delete _internal;
}

void TerrainIterator::operator++() {
    _internal->_it++;
}

std::pair<long, Terrain*> TerrainIterator::operator*() {
    auto &p = *_internal->_it;
    return std::make_pair(p.first, &p.second);
}

bool TerrainIterator::hasNext() const {
    return _internal->_it != _collector._terrains.end();
}


FlatWorldCollector::FlatWorldCollector() = default;
FlatWorldCollector::~FlatWorldCollector() = default;

void FlatWorldCollector::reset() {
    WorldCollector::reset();

    _terrains.clear();
}

void FlatWorldCollector::collect(FlatWorld &world, WorldZone &zone) {
    WorldCollector::collect(world, zone);

    Ground &ground = world.ground();
    ground.collectChunk(*this, world, zone);
}

void FlatWorldCollector::addTerrain(long key, const Terrain &terrain) {
    _terrains.emplace(key, terrain);
}

TerrainIterator FlatWorldCollector::iterateTerrains() {
    return TerrainIterator(*this);
}