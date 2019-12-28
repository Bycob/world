#ifndef WORLD_MAPFILTEREDDISTRIBUTION_H
#define WORLD_MAPFILTEREDDISTRIBUTION_H

#include "world/core/WorldConfig.h"

#include "world/core/InstanceDistribution.h"

namespace world {

class IDistributionMapProvider {
public:
    virtual ~IDistributionMapProvider() = default;

    virtual double getValueAt(const vec3d &pos, int layer) = 0;
};

template <typename TDistribution>
class WORLDAPI_EXPORT MapFilteredDistribution : public TDistribution {
public:
    using Position = DistributionBase::Position;

    template <typename... Args>
    MapFilteredDistribution(IDistributionMapProvider *mapProvider,
                            Args &&... args)
            : _mapProvider(mapProvider), TDistribution(args...) {
        if (_mapProvider == nullptr) {
            throw std::runtime_error("Map provider is null, and that is bad");
        }
    }

    std::vector<Position> getPositions(Chunk &chunk) {
        std::vector<Position> positions = TDistribution::getPositions(chunk);
        std::vector<Position> newPositions;

        std::uniform_real_distribution<double> distrib(0, 1);

        for (auto &position : positions) {
            double value = _mapProvider->getValueAt(position._pos, _layer);

            if (distrib(DistributionBase::_rng) < value) {
                newPositions.push_back(position);
            }
        }

        return newPositions;
    }

    void setLayer(int layer) { _layer = layer; }

private:
    IDistributionMapProvider *_mapProvider;

    // Temporary solution to identify the layer in the IDistributionMapProvider
    // The same layer is currently used for all the generators
    int _layer = 0;
};

} // namespace world

#endif // WORLD_MAPFILTEREDDISTRIBUTION_H
