#include "SeedDistribution.h"

#include "world/math/RandomHelper.h"
#include "world/math/Interpolation.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(DistributionBase, SeedDistribution,
                           "SeedDistribution");

void SeedDistribution::addSeeds(Chunk &chunk) {
    if (_habitats.empty())
        throw std::runtime_error(
            "[SeedDistribution] No generator available for new seeds!");

    auto bounds = getBounds(chunk);
    double area = _tileSize * _tileSize / 1e6;
    std::uniform_real_distribution<double> distrib(0, 1);
    std::uniform_int_distribution<int> genDistrib(0, _habitats.size() - 1);

    for (int y = bounds.first.y; y <= bounds.second.y; ++y) {
        for (int x = bounds.first.x; x <= bounds.second.x; ++x) {
            vec2i tileCoords{x, y};
            auto ret = _seeds.insert({tileCoords, std::vector<Seed>()});

            if (ret.second) {
                auto &seeds = ret.first->second;
                double count = randRound(_rng, area * _seedDensity);

                for (int i = 0; i < count; ++i) {
                    vec2d seedPos =
                        (vec2d{distrib(_rng), distrib(_rng)} + tileCoords) *
                        _tileSize;
                    double distRatio = distrib(_rng);
                    double distance = _maxDist * (1 - distRatio * distRatio);

                    // Choose the generator
                    // TODO choose the generator according to local conditions
                    u32 generatorId = genDistrib(_rng);
                    seeds.push_back({seedPos, generatorId, distance});

                    // std::cout << seedPos.x << " " << seedPos.y << " " <<
                    // distance << " " << generatorId << std::endl;
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

std::vector<SeedDistribution::Position> SeedDistribution::getPositions(
    Chunk &chunk, const ExplorationContext &ctx) {
    addSeeds(chunk);

    std::vector<Position> positions;
    std::vector<Seed> seedsAround = getSeedsAround(chunk);

    if (seedsAround.empty()) {
        std::cerr << "[WARN] No seed found in the region. Author is required "
                     "to fix his algorithm";
        return positions;
    }
    // We assume that seedsAround.size() != 0

    // Find the highest possible density
    double maxDensity = 0;

    for (Seed &seed : seedsAround) {
        double density = _habitats.at(seed._generatorId)._density;

        if (density > maxDensity) {
            maxDensity = density;
        }
    }

    // Get max number of instance in the chunk, so we can generate
    // as much random positions as needed
    vec3d chunkPos = chunk.getPosition3D();
    vec3d chunkDims = chunk.getSize();
    double chunkArea = chunkDims.x * chunkDims.y;
    int count = randRound(_rng, maxDensity * chunkArea);

    std::uniform_real_distribution<double> posDistrib(0, 1);
    std::uniform_real_distribution<double> keepDistrib(0, 1);

    // For each position, species will compete with each others
    // <!> This algorithm considers that a species competes for the habitat
    // even if it is not adapted to it.
    for (int i = 0; i < count; ++i) {
        // Get 3D position (with altitude)
        vec3d position{posDistrib(_rng) * chunkDims.x,
                       posDistrib(_rng) * chunkDims.y, -10000};
        vec3d absPos = ctx.getEnvironment().findNearestFreePoint(
            chunkPos + position, vec3d{0, 0, 1}, _resolution,
            ExplorationContext::getDefault());
        position = absPos - chunkPos;

        if (position.z < 0 || position.z >= chunkDims.z) {
            continue;
        }

        // Compute chance for each species to win the place
        std::vector<double> habitatCoefs;
        habitatCoefs.reserve(seedsAround.size());

        std::vector<double> distanceCoefs;
        distanceCoefs.reserve(seedsAround.size());

        double total = 0;

        // This lambda compute the chance of surviving at x if species life zone
        // is range
        const auto getProb = [](vec2d range, double x) {
            double l = (range.y - range.x) * 0.1;
            return min(smoothstep(range.x - l, range.x + l, x),
                       smoothstep(range.y + l, range.y - l, x));
        };

        for (auto &seed : seedsAround) {
            HabitatFeatures &habitat = _habitats.at(seed._generatorId);
            double habitatCoef = 1;

            if (absPos.z < 0 && !habitat._sea) {
                habitatCoef = 0;
            }

            habitatCoef *= getProb(habitat._altitude, absPos.z);
            // TODO add humidity and temperature
            // double humidity = _env->getHumidity(position);
            // double temperature = _env->getTemperature(position);

            habitatCoefs.push_back(habitatCoef);

            double distance = seed._position.length(vec2d(absPos));
            double distanceCoef = pow(2, -distance / seed._distance) / 2;
            distanceCoefs.push_back(distanceCoef);
            // TODO (advanced) introduce environmental obstacles

            total += habitatCoef * distanceCoef;
        }

        // Select seed according to previously computed probabilities
        double selector = keepDistrib(_rng) * total;
        int selectedSeedID = -1;

        double sum = 0;
        while (sum <= selector &&
               selectedSeedID < int(seedsAround.size()) - 1) {
            // without int() cast, above expression is always false
            selectedSeedID++;
            sum += habitatCoefs[selectedSeedID];
        }

        // Once a seed is chosen, we still filter the point according to the
        // chosen species density
        Seed &seed = seedsAround[selectedSeedID];
        HabitatFeatures &habitat = _habitats[seed._generatorId];
        double keepRate =
            habitatCoefs[selectedSeedID] * habitat._density / maxDensity;

        if (keepDistrib(_rng) <= keepRate) {
            positions.push_back({position, int(seed._generatorId)});
        }
    }

    return positions;
}

void SeedDistribution::write(WorldFile &wf) const {
    DistributionBase::write(wf);
    wf.addDouble("tileSize", _tileSize);
    wf.addDouble("seedDensity", _seedDensity);
    wf.addDouble("seedAmount", _seedAmount);
    wf.addDouble("maxDist", _maxDist);
}

void SeedDistribution::read(const WorldFile &wf) {
    DistributionBase::read(wf);
    wf.readDoubleOpt("tileSize", _tileSize);
    wf.readDoubleOpt("seedDensity", _seedDensity);
    wf.readDoubleOpt("seedAmount", _seedAmount);
    wf.readDoubleOpt("maxDist", _maxDist);
}

std::pair<vec2i, vec2i> SeedDistribution::getBounds(const Chunk &chunk) const {
    vec2d lower = static_cast<vec2d>(chunk.getPosition3D()) - vec2d{_maxDist};
    vec2d upper = static_cast<vec2d>(chunk.getPosition3D() + chunk.getSize()) +
                  vec2d{_maxDist};
    return {(lower / _tileSize).floor(), (upper / _tileSize).ceil()};
}

} // namespace world
