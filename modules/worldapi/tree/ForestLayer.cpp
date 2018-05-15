#include "ForestLayer.h"

#include "Tree.h"
#include "TreeSkelettonGenerator.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"

namespace world {

ForestLayer::ForestLayer() : _rng(static_cast<u64>(time(NULL))) {}

void ForestLayer::decorate(FlatWorld &world, const WorldZone &zone) {
    // Check resolution
    const double resolution = 5;
    const double minres = zone.getInfo().getMinResolution();
    const double maxres = zone.getInfo().getMaxResolution();

    if (!(resolution >= minres && resolution < maxres)) {
        return;
    }

    IGround &ground = world.ground();

    // Compute area
    vec3d zoneSize = zone.getInfo().getDimensions();
    vec3d zoneOffset = zone.getInfo().getAbsoluteOffset();
    const double area = zoneSize.x * zoneSize.y;

    // Max tree count
    std::uniform_real_distribution<double> stddistrib(0, 1);
    double maxTreeCountf = _maxDensity * area / 1e6;
    u32 maxTreeCount = static_cast<u32>(floor(maxTreeCountf));

    if (stddistrib(_rng) < maxTreeCountf - maxTreeCount) {
        maxTreeCount++;
    }

    // Generate random points
    std::vector<vec2d> randomPoints;
    randomPoints.reserve(maxTreeCount);

    // TODO use a method that includes a minimal distance between points
    std::uniform_real_distribution<double> xdistrib(0, zoneSize.x);
    std::uniform_real_distribution<double> ydistrib(0, zoneSize.y);

    for (int i = 0; i < maxTreeCount; ++i) {
        randomPoints.emplace_back(xdistrib(_rng), ydistrib(_rng));
    }

    // Populate trees
    for (auto &pt : randomPoints) {
        const double altitude = ground.observeAltitudeAt(zone, pt.x, pt.y);

        // skip if altitude is not in this chunk
        if (altitude < zoneOffset.z || altitude > zoneOffset.z + zoneSize.z) {
            continue;
        }

        if (stddistrib(_rng) < getDensityAtAltitude(altitude)) {
            vec3d pos{pt.x, pt.y, altitude - zoneOffset.z};
            addTree(world, zone, pos);
        }
    }
}

void ForestLayer::addTree(FlatWorld &world, const WorldZone &zone,
                          const vec3d &position) {
    Tree &tree = world.addObject<Tree>(zone);
    tree.setPosition3D(position);

    auto &skeletton = tree.addWorker<TreeSkelettonGenerator>();
    skeletton.setRootWeight(TreeParamsd::gaussian(3, 0.2));
    skeletton.setForkingCount(
        TreeParamsi::WeightThreshold(0.15, TreeParamsi::uniform_int(3, 4)));
    skeletton.setInclination(
        TreeParamsd::PhiOffset(TreeParamsd::gaussian(0.2 * M_PI, 0.05 * M_PI)));
    skeletton.setTheta(
        TreeParamsd::UniformTheta(TreeParamsd::gaussian(0, 0.05 * M_PI)));
    skeletton.setSize(
        TreeParamsd::SizeFactor(TreeParamsd::uniform_real(0.5, 0.75)));

    tree.addWorker<TrunkGenerator>(12);
    tree.addWorker<LeavesGenerator>(0.2, 0.15);
}

double ForestLayer::getDensityAtAltitude(double altitude) {
    // [,0] -> 0; 100 -> 0.5; 500 -> 0.25; 1000 -> 0.12; 2000 -> 0;
    // TODO utiliser des splines
    if (altitude <= 0) {
        return 0;
    } else if (altitude <= 100) {
        double x = altitude / 100;
        return x * 0.5;
    } else if (altitude <= 500) {
        double x = (altitude - 100) / 400;
        return (1 - x) * 0.5 + x * 0.25;
    } else if (altitude <= 1000) {
        double x = (altitude - 500) / 500;
        return (1 - x) * 0.25 + x * 0.12;
    } else if (altitude <= 2000) {
        double x = (altitude - 1000) / 1000;
        return (1 - x) * 0.12;
    } else
        return 0;
}

} // namespace world
