#include "FirstPersonExplorer.h"

#include "math/MathsHelper.h"

namespace world {

FirstPersonExplorer::FirstPersonExplorer(double eyeResolution, double fov,
                                         double punctumProximum)
        : _eyeResolution(eyeResolution), _fov(fov),
          _punctumProximum(punctumProximum), _farDistance(5000),
          _position({0, 0, 0}) {}

void FirstPersonExplorer::setPosition(const vec3d &position) {
    _position = position;
}

void FirstPersonExplorer::setEyeResolution(double resolution) {
    _eyeResolution = resolution;
}

void FirstPersonExplorer::setFOV(double fov) { _fov = fov; }

void FirstPersonExplorer::setPunctumProximum(double punctumProximum) {
    _punctumProximum = punctumProximum;
}

void FirstPersonExplorer::setFarDistance(double maxDistance) {
    _farDistance = maxDistance;
}

vec3d FirstPersonExplorer::getChunkNearestPoint(const WorldZone &zone) {
    vec3d lower = zone->getAbsoluteOffset();
    vec3d upper = lower + zone->getChunk().getSize();

    return {clamp(_position.x, lower.x, upper.x),
            clamp(_position.y, lower.y, upper.y),
            clamp(_position.z, lower.z, upper.z)};
}

double FirstPersonExplorer::getResolutionAt(const vec3d &pos) {
    double length = max(_punctumProximum, _position.length(pos));
    // _fov * length can be seen as the "image size"
    return _eyeResolution / (_fov * M_PI / 180 * length);
}

void FirstPersonExplorer::explore(World &world, ExplorationResult &result) {
    std::set<WorldZone> explored;
    std::set<WorldZone> toExplore;
    toExplore.insert(world.exploreLocation(_position));

    while (!toExplore.empty()) {
        auto it = toExplore.begin();

        WorldZone currentZone = *it;
        const Chunk &currentChunk = (*it)->getChunk();

        // Vertical exploration : we explore the inside
        exploreVertical(world, *it, result);

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

void FirstPersonExplorer::exploreVertical(World &world, const WorldZone &zone,
                                          ExplorationResult &result) {

    const Chunk &currentChunk = zone->getChunk();
    const double resolution = getResolutionAt(getChunkNearestPoint(zone));

    if (currentChunk.getMaxResolution() < resolution) {
        auto smallerZones = world.exploreInside(zone);

        for (WorldZone &smallerZone : smallerZones) {
            exploreVertical(world, smallerZone, result);
        }
    } else {
        result.appendZone(zone);
    }
}
} // namespace world
