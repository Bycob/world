
#include "Chunk.h"

namespace world {
    template<typename T, typename C>
    void FirstPersonExplorer::explore(T &world, C &collector) {
        std::set<WorldZone> explored;
        std::set<WorldZone> toExplore;
        toExplore.insert(world.exploreLocation(_origin));

        while (!toExplore.empty()) {
            auto it = toExplore.begin();

            WorldZone currentZone = *it;
            const Chunk &currentChunk = (*it)->getChunk();

            // Retrieve content
            world.collect(currentZone, collector);

            // Vertical exploration : we explore the inside
            exploreVertical(world, *it, collector);

            // Horizontal exploration : we explore the neighbourhood
            vec3i directions[] = {
                    {1,  0,  0},
                    {-1, 0,  0},
                    {0,  1,  0},
                    {0,  -1, 0},
                    {0,  0,  1},
                    {0,  0,  -1}
            };

            for (vec3i direction : directions) {
                WorldZone neighbour = world.exploreNeighbour(*it, direction);
                auto offset = getChunkNearestPoint(neighbour);

                if (explored.find(neighbour) == explored.end()
                    && _origin.squaredLength(offset) < _maxDistance * _maxDistance) {

                    toExplore.insert(neighbour);
                }
            }

            explored.insert(*it);
            toExplore.erase(it);
        }
    }

    template<typename T, typename C>
    void FirstPersonExplorer::exploreVertical(T &world, const WorldZone &zone, C &collector) {
        const Chunk &currentChunk = zone->getChunk();
        const double detailSize = getDetailSizeAt(getChunkNearestPoint(zone));
        //std::cout << detailSize << std::endl;

        if (currentChunk.getMinDetailSize() > detailSize) {
            auto smallerZones = world.exploreInside(zone);

            for (WorldZone &smallerZone : smallerZones) {
                world.collect(smallerZone, collector);

                exploreVertical(world, smallerZone, collector);
            }
        }
    }
}
