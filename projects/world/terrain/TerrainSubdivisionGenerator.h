#ifndef WORLD_TERRAINSUBDIVISIONGENERATOR_H
#define WORLD_TERRAINSUBDIVISIONGENERATOR_H

#include "world/core/WorldConfig.h"

#include <map>

#include <armadillo/armadillo>

#include "TerrainSubdivisionTree.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT TerrainSubdivisionGenerator {
public:
    TerrainSubdivisionGenerator();

    TerrainSubdivisionTree *generateSubdivisions(Terrain &terrain,
                                                 int subdivideFactor,
                                                 int subdivisionsCount);

    void generateSubdivisionLevels(TerrainSubdivisionTree &tree,
                                   int subdivideFactor, int subdivisionsCount);

    virtual void generateSubdivisionLevel(TerrainSubdivisionTree &tree,
                                          int subdivideFactor);

protected:
    virtual void generateSubdivision(TerrainSubdivisionTree &terrain, int x,
                                     int y);

private:
    std::unique_ptr<ITerrainWorker> _generator;
    mutable std::map<std::pair<int, int>, arma::Mat<double>> _buffer;

    // -------
    arma::mat &getBuf(int x, int y) const;

    template <typename... Args>
    arma::Mat<double> &createInBuf(int x, int y, Args &&... args) const {
        std::pair<int, int> index = std::make_pair(x, y);
        _buffer[index] = arma::Mat<double>(args...);
        return _buffer[index];
    }

    void adaptFromBuffer(TerrainSubdivisionTree &terrain, int xsub,
                         int ysub) const;
};
} // namespace world


#endif // WORLD_TERRAINSUBDIVISIONGENERATOR_H
