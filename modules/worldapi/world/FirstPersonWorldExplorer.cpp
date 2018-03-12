#include "FirstPersonWorldExplorer.h"

#include "../maths/MathsHelper.h"

using namespace maths;

FirstPersonWorldExplorer::FirstPersonWorldExplorer(double minAngle, double punctumProximum)
        : _origin({0, 0, 0}), _angularResolution(minAngle), _punctumProximum(punctumProximum) {

}

void FirstPersonWorldExplorer::setOrigin(const maths::vec3d &origin) {
    _origin = origin;
}

void FirstPersonWorldExplorer::setMinAngle(double minAngle) {
    _angularResolution = minAngle;
}

void FirstPersonWorldExplorer::setPunctumProximum(double punctumProximum) {
    _punctumProximum = punctumProximum;
}

double FirstPersonWorldExplorer::getDetailSizeAt(const vec3d &pos) {
    double length = max(_punctumProximum, _origin.length(pos));
    return _angularResolution * length;
}

// TODO cette méthode en template ! (Avec IWorldCollector<T>)
void FirstPersonWorldExplorer::explore(World &world, WorldCollector &collector) {
    std::set<ChunkNode> explored;
    std::set<ChunkNode> toExplore;
    toExplore.insert(world.exploreLocation(_origin));

    while (!toExplore.empty()) {
        auto it = toExplore.begin();

        ChunkNode currentChunk = *it;

        // Retrieve content
        collector.collect(world, currentChunk);

        // Vertical exploration : we explore the inside


        // Horizontal exploration : we explore the neighbourhood
        vec3i directions[] = {
            {1, 0, 0},
            {-1, 0, 0},
            {0, 1, 0},
            {0, -1, 0},
            {0, 0, 1},
            {0, 0, -1}
        };

        for (vec3i direction : directions) {
            ChunkNode neighbour = world.exploreNeighbour(*it, direction);
            auto offset = neighbour._chunk.getOffset();
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