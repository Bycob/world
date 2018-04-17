#include <chrono>

#include "Chunk.h"

namespace world {
template <typename T, typename C>
void FirstPersonExplorer::exploreAndCollect(T &world, C &collector) {
    // Get the zone to collect
    ExplorationResult result;
    explore(world, result);

    // Collect
    auto end = result.end();
    for (auto it = result.begin(); it != end; ++it) {
        world.collect(*it, collector);
    }
}
} // namespace world
