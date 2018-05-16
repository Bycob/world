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
    vec3d upper = lower + zone->getDimensions();

    return {clamp(_position.x, lower.x, upper.x),
            clamp(_position.y, lower.y, upper.y),
            clamp(_position.z, lower.z, upper.z)};
}

double FirstPersonExplorer::getResolutionAt(const vec3d &pos) const {
    double length = max(_punctumProximum, _position.length(pos));
    // _fov * length can be seen as the "image size"
    return _eyeResolution / (_fov * M_PI / 180 * length);
}

double FirstPersonExplorer::getResolutionAt(const WorldZone &zone,
                                            const vec3d &pos) const {
    return getResolutionAt(zone.getInfo().getAbsoluteOffset() + pos);
}

void FirstPersonExplorer::explore(World &world, ExplorationResult &result) {
    std::set<WorldZone> explored;
    std::set<WorldZone> toExplore;
    toExplore.insert(world.exploreLocation(_position));

    while (!toExplore.empty()) {
        auto it = toExplore.begin();

        const WorldZone &currentZone = *it;

        // Vertical exploration : we explore the inside
        exploreVertical(world, *it, result);

        // Horizontal exploration : we explore the neighbourhood
        auto neighbourhood = world.exploreNeighbours(currentZone);

        for (auto &neighbour : neighbourhood) {
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

    result.appendZone(zone);
    const double resolution = getResolutionAt(getChunkNearestPoint(zone));

    if (zone->getMaxResolution() < resolution) {
        auto smallerZones = world.exploreInside(zone);

        for (WorldZone &smallerZone : smallerZones) {
            exploreVertical(world, smallerZone, result);
        }
    }
}
} // namespace world
