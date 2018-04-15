#include "TerrainSubdivisionGenerator.h"

namespace world {

using namespace arma;

TerrainSubdivisionGenerator::TerrainSubdivisionGenerator()
        : _buffer(std::map<std::pair<int, int>, arma::Mat<double>>()) {}

TerrainSubdivisionTree *TerrainSubdivisionGenerator::generateSubdivisions(
    Terrain &terrain, int subdivideFactor, int subdivisionsCount) {
    auto *result = new TerrainSubdivisionTree(terrain);
    generateSubdivisionLevels(*result, subdivideFactor, subdivisionsCount);
    return result;
}

void TerrainSubdivisionGenerator::generateSubdivisionLevels(
    TerrainSubdivisionTree &tree, int subdivideFactor, int subdivisionsCount) {
    generateSubdivisionLevel(tree, subdivideFactor);

    // Subdivisions d'un niveau supplémentaire (récursivité)
    if (subdivisionsCount > 1) {
        for (int x = 0; x < subdivideFactor; x++) {
            for (int y = 0; y < subdivideFactor; y++) {
                TerrainSubdivisionTree &subtree = tree.getSubtree(x, y);
                generateSubdivisionLevels(subtree, subdivideFactor,
                                          subdivisionsCount - 1);
            }
        }
    }
}

void TerrainSubdivisionGenerator::generateSubdivisionLevel(
    TerrainSubdivisionTree &terrain, int subdivideFactor) {
    terrain.subdivide(subdivideFactor);

    // Pas de prébuffering

    // Géneration des buffers pour le niveau de détail actuel
    for (int x = 0; x < subdivideFactor; x++) {
        for (int y = 0; y < subdivideFactor; y++) {
            generateSubdivision(terrain, x, y);
        }
    }

    // Conversion des buffers en terrain
    for (int x = 0; x < subdivideFactor; x++) {
        for (int y = 0; y < subdivideFactor; y++) {
            adaptFromBuffer(terrain, x, y);
        }
    }

    _buffer.clear();
}

void TerrainSubdivisionGenerator::generateSubdivision(
    TerrainSubdivisionTree &terrain, int xsub, int ysub) {
    // TODO Faire plusieurs tests pour voir si on obtient un meilleur résultat
    // en changeant les paramètres de l'algo de perlin.
    /*mat &mat = createInBuf(xsub, ysub, terrain.getSubtree(xsub,
    ysub)._terrain->_array); _perlin.generatePerlinNoise2D(mat, _offset,
    _octaveCount, _frequency, _persistence);

    double oneTerrainLength = 1.0 / terrain._subdivideFactor;

    // Harmonisation horizontale
    // -> Le joint en y doit toujours se faire avant le joint en x pour
    n'importe quel terrain if (ysub != 0) { _perlin.join(getBuf(xsub, ysub - 1),
                     mat,
                     Direction::AXIS_Y,
                     _octaveCount, _frequency, _persistence);

        if (xsub != 0) {
            _perlin.join(getBuf(xsub - 1, ysub - 1),
                         getBuf(xsub, ysub - 1),
                         Direction::AXIS_X,
                         _octaveCount, _frequency, _persistence, true);
        }
    }
    if (xsub == terrain._subdivideFactor - 1) {
        _perlin.join(getBuf(xsub - 1, ysub),
                     mat,
                     Direction::AXIS_X,
                     _octaveCount, _frequency, _persistence, true);
    }*/
}

void TerrainSubdivisionGenerator::adaptFromBuffer(TerrainSubdivisionTree &tree,
                                                  int xsub, int ysub) const {
    /*TerrainSubdivisionTree &subtree = tree.getSubtree(xsub, ysub);
    Terrain &subterrain = *subtree._terrain;
    Mat<double> &mat = getBuf(xsub, ysub);

    int rows = subterrain._array.n_rows;
    int cols = subterrain._array.n_cols;
    int sf = tree._subdivideFactor;

    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            subterrain._array(x, y) = mat(x, y) * _subdivNoiseRatio
                                      + (1 - _subdivNoiseRatio) *
    tree.getZInterpolated(
                    ((double) xsub + (double) x / rows) / sf,
                    ((double) ysub + (double) y / cols) / sf);
        }
    }

    subtree._noisePart = _subdivNoiseRatio;*/
}

arma::Mat<double> &TerrainSubdivisionGenerator::getBuf(int x, int y) const {
    return _buffer.at(std::make_pair(x, y));
}
} // namespace world