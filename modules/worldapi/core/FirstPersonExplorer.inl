#include <chrono>

#include "Chunk.h"
#include "Profiler.h"

namespace world {
template <typename T, typename C>
void FirstPersonExplorer::exploreAndCollect(T &world, C &collector) {
    Profiler profiler;

    // Get the zone to collect
    profiler.endStartSection("explore");
    ExplorationResult result;
    explore(world, result);

    // Collect
    profiler.endStartSection("collect");
    auto end = result.end();
    for (auto it = result.begin(); it != end; ++it) {
        world.collect(*it, collector, *this);
    }
    profiler.endSection();

    profiler.dump();
}
} // namespace world
