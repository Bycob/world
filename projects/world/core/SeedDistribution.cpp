#include "SeedDistribution.h"

#include "world/math/RandomHelper.h"
#include "world/math/Interpolation.h"

namespace world {

void SeedDistribution::addSeeds(Chunk &chunk) {
    auto bounds = getBounds(chunk);
    double area = _tileSize * _tileSize / 1e6;
    std::uniform_real_distribution<double> distrib(0, 1);

    for (int y = bounds.first.y; y <= bounds.second.y; ++y) {
        for (int x = bounds.first.x; x <= bounds.second.x; ++x) {
            auto ret = _seeds.insert({{x, y}, std::vector<Seed>()});

            if (ret.second) {
                auto &seeds = ret.first->second;
                double count = randRound(_rng, area * _seedDensity);

                for (int i = 0; i < count; ++i) {
                    vec2d seedPos =
                        vec2d{distrib(_rng), distrib(_rng)} * _tileSize;
                    double distRatio = distrib(_rng);
                    double distance = _maxDist * (1 - distRatio * distRatio);

                    // Choose the generator
                    u32 generatorId = 0; // TODO
                    seeds.push_back({seedPos, generatorId, distance});
                }
            }
        }
    }
}

std::vector<Seed> SeedDistribution::getSeedsAround(Chunk &chunk) {
    std::vector<Seed> seeds;
    auto bounds = getBounds(chunk);

    for (int y = bounds.first.y; y <= bounds.second.y; ++y) {
        for (int x = bounds.first.x; x <= bounds.second.x; ++x) {
            auto it = _seeds.find({x, y});
            if (it != _seeds.end()) {
                seeds.insert(seeds.end(), it->second.begin(), it->second.end());
            }
        }
    }

    return seeds;
}

std::vector<vec3d> SeedDistribution::getPositions(Chunk &chunk,
                                                  int generatorId) {
    std::vector<vec3d> positions;
    std::vector<Seed> seedsAround;
    HabitatFeatures &habitat = _habitats.at(generatorId);

    // Keep only the seeds for the generator from parameters
    for (Seed seed : getSeedsAround(chunk)) {
        if (seed._generatorId == generatorId) {
            seedsAround.emplace_back(std::move(seed));
        }
    }

    vec3d chunkPos = chunk.getPosition3D();
    vec3d chunkDims = chunk.getSize();
    double chunkArea = chunkDims.x * chunkDims.y;
    int count = randRound(_rng, habitat._density * chunkArea);

    std::uniform_real_distribution<double> posDistrib(0, 1);
    std::uniform_real_distribution<double> keepDistrib(0, 1);

    for (int i = 0; i < count; ++i) {
        vec3d position{posDistrib(_rng) * chunkDims.x,
                       posDistrib(_rng) * chunkDims.y, -10000};
        vec3d absPos = _env->findNearestFreePoint(
            chunkPos + position, vec3d{0, 0, 1}, _resolution,
            ExplorationContext::getDefault());
        position = absPos - chunkPos;

        if (position.z < 0 || position.z >= chunkDims.z) {
            continue;
        }

        if (absPos.z < 0 && !habitat._sea) {
            continue;
        }

        // drop according to local conditions
        auto getProb = [](vec2d range, double x) {
            double l = (range.y - range.x) * 0.1;
            return min(smoothstep(range.x - l, range.x + l, x),
                       smoothstep(range.y + l, range.y - l, x));
        };
        double keep = 1;
        // double humidity = _env->getHumidity(position);
        // double temperature = _env->getTemperature(position);
        keep *= getProb(habitat._altitude, absPos.z);

        double denseProp = 0;
        for (auto &seed : seedsAround) {
            double distance = seed._position.length(vec2d(absPos));
            double ratio = max(0, distance / seed._distance);
            denseProp += ratio * ratio;
        }
        keep *= min(denseProp, 1);

        if (keepDistrib(_rng) <= keep) {
            positions.push_back(position);
        }
    }

    return positions;
}

std::pair<vec2i, vec2i> SeedDistribution::getBounds(const Chunk &chunk) const {
    vec2d lower = static_cast<vec2d>(chunk.getPosition3D()) - vec2d{_maxDist};
    vec2d upper = static_cast<vec2d>(chunk.getPosition3D() + chunk.getSize()) +
                  vec2d{_maxDist};
    return {(lower / _tileSize).floor(), (upper / _tileSize).ceil()};
}

} // namespace world
