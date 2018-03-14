
template <typename T>
void FirstPersonWorldExplorer::explore(T &world, IWorldCollector<T> &collector) {
    std::set<WorldZone> explored;
    std::set<WorldZone> toExplore;
    toExplore.insert(world.exploreLocation(_origin));

    while (!toExplore.empty()) {
        auto it = toExplore.begin();

        WorldZone currentChunk = *it;

        // Retrieve content
        collector.collect(world, currentChunk);

        // Vertical exploration : we explore the inside


        // Horizontal exploration : we explore the neighbourhood
        maths::vec3i directions[] = {
                {1, 0, 0},
                {-1, 0, 0},
                {0, 1, 0},
                {0, -1, 0},
                {0, 0, 1},
                {0, 0, -1}
        };

        for (maths::vec3i direction : directions) {
            WorldZone neighbour = world.exploreNeighbour(*it, direction);
            auto offset = neighbour.chunk().getOffset();
            double detailSize = getDetailSizeAt(offset);

            // TODO Precise exit condition
            if (explored.find(neighbour) == explored.end() && detailSize < 200) {
                toExplore.insert(neighbour);
            }
        }

        explored.insert(*it);
        toExplore.erase(it);
    }
}