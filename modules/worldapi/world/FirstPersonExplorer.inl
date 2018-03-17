
namespace world {
    template<typename T>
    void FirstPersonExplorer::explore(T &world, ICollector<T> &collector) {
        std::set<WorldZone> explored;
        std::set<WorldZone> toExplore;
        toExplore.insert(world.exploreLocation(_origin));

        while (!toExplore.empty()) {
            auto it = toExplore.begin();

            WorldZone currentZone = *it;
            const Chunk &currentChunk = (*it)->getChunk();

            // Retrieve content
            collector.collect(world, currentZone);

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

    template<typename T>
    void FirstPersonExplorer::exploreVertical(T &world, const WorldZone &zone, ICollector<T> &collector) {
        const Chunk &currentChunk = zone->getChunk();
        const double detailSize = getDetailSizeAt(getChunkNearestPoint(zone));
        //std::cout << detailSize << std::endl;

        if (currentChunk.getMinDetailSize() > detailSize) {
            auto smallerZones = world.exploreInside(zone);

            for (WorldZone &smallerZone : smallerZones) {
                collector.collect(world, smallerZone);

                exploreVertical(world, smallerZone, collector);
            }
        }
    }
}
