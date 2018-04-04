#ifndef WORLD_SIMPLETERRAINTEXTUREGENERATOR_H
#define WORLD_SIMPLETERRAINTEXTUREGENERATOR_H

#include "core/WorldConfig.h"

#include <armadillo/armadillo>

#include "ITerrainWorker.h"
#include "assets/Image.h"
#include "TerrainTexmapBuilder.h"

namespace world {
    class WORLDAPI_EXPORT TerrainTexmapModifier {
    public:
        Image generateTexture(const Terrain &terrain, const TerrainTexmapBuilder &builder,
                              const arma::Mat<double> &randomArray) const;

        Image generateTexture(const Terrain &terrain, const arma::Cube<double> &map,
                              const arma::Mat<double> &randomArray) const;
    };
}

#endif //WORLD_SIMPLETERRAINTEXTUREGENERATOR_H
