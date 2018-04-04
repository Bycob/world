#include "TerrainTexmapModifier.h"

using namespace arma;

namespace world {
    //TODO check des tailles des tableaux passés en paramètres (map et randomArray)

    Image TerrainTexmapModifier::generateTexture(const Terrain &terrain, const TerrainTexmapBuilder &builder,
                                            const Mat<double> &randomArray) const {
        //TODO support de l'altitude
        auto map = builder.convertToMap();

        return generateTexture(terrain, map, randomArray);
    }

    //Cette méthode là ne supportera pas l'altitude.
    Image TerrainTexmapModifier::generateTexture(const Terrain &terrain, const arma::Cube<double> &map,
                                            const Mat<double> &randomArray) const {
        float scaleX = (float) terrain.getResolution() / randomArray.n_rows;
        float scaleY = (float) terrain.getResolution() / randomArray.n_cols;
        arma::Cube<double> result(randomArray.n_rows, randomArray.n_cols, 3);

        for (int x = 0; x < randomArray.n_rows; x++) {
            for (int y = 0; y < randomArray.n_cols; y++) {
                double x1 = (double) x / randomArray.n_rows;
                double y1 = (double) y / randomArray.n_cols;

                double z = terrain.getExactHeightAt(x1, y1);
                double randParam = randomArray(x, y);

                result(x, y, 0) = map((int) (z * (RESOLUTION - 1)), (int) (randParam * (RESOLUTION - 1)), 0);
                result(x, y, 1) = map((int) (z * (RESOLUTION - 1)), (int) (randParam * (RESOLUTION - 1)), 1);
                result(x, y, 2) = map((int) (z * (RESOLUTION - 1)), (int) (randParam * (RESOLUTION - 1)), 2);
            }
        }

        return Image(result);
    }
}
