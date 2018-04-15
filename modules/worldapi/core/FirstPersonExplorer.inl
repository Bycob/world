
#include "Chunk.h"

namespace world {
template <typename T, typename C>
void FirstPersonExplorer::explore(T &world, C &collector) {
    std::set<WorldZone> explored;
    std::set<WorldZone> toExplore;
    toExplore.insert(world.exploreLocation(_position));

    while (!toExplore.empty()) {
        auto it = toExplore.begin();

        WorldZone currentZone = *it;
        const Chunk &currentChunk = (*it)->getChunk();

        // Retrieve content
        world.collect(currentZone, collector);

        // Vertical exploration : we explore the inside
        exploreVertical(world, *it, collector);

        // Horizontal exploration : we explore the neighbourhood
        vec3i directions[] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                              {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};

        for (vec3i direction : directions) {
            WorldZone neighbour = world.exploreNeighbour(*it, direction);
            auto offset = getChunkNearestPoint(neighbour);

            if (explored.find(neighbour) == explored.end() &&
                _position.squaredLength(offset) < _farDistance * _farDistance) {

                toExplore.insert(neighbour);
            }
        }

        explored.insert(*it);
        toExplore.erase(it);
    }
}

template <typename T, typename C>
void FirstPersonExplorer::exploreVertical(T &world, const WorldZone &zone,
                                          C &collector) {
    const Chunk &currentChunk = zone->getChunk();
    const double resolution = getResolutionAt(getChunkNearestPoint(zone));

    if (currentChunk.getMaxResolution() < resolution) {
        auto smallerZones = world.exploreInside(zone);

        for (WorldZone &smallerZone : smallerZones) {
            world.collect(smallerZone, collector);

            exploreVertical(world, smallerZone, collector);
        }
    }
}
} // namespace world
