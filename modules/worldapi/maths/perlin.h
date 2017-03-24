/* Ce fichier contient des fonctions utiles pour générer du bruit de perlin.
Il se base sur la bibliothèque matricielle armadillo.*/
#pragma once

#include <armadillo/armadillo>

#include <worldapi/worldapidef.h>

namespace perlin {

    WORLDAPI_EXPORT void generatePerlinNoise2D(arma::Mat<double> &output,
                                               int offset,
                                               int octaves,
                                               float frequency,
                                               float persistence,
                                               bool repeatable = false);

    WORLDAPI_EXPORT arma::Mat<double> generatePerlinNoise2D(int size,
                                                            int offset,
                                                            int octaves,
                                                            float frequency,
                                                            float persistence,
                                                            bool repeatable = false);

    void generatePerlinOctave(arma::Mat<double> &output,
                              int offset,
                              float frequency,
                              bool repeatable);

}