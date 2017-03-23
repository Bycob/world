/* Ce fichier contient des fonctions utiles pour générer du bruit de perlin.
Il se base sur la bibliothèque matricielle armadillo.*/
#pragma once

#include <worldapi/worldapidef.h>

namespace arma {
	template <typename T> class Mat;
}

WORLDAPI_EXPORT void generatePerlinNoise2D(arma::Mat<double> &output, int offset, int octaves, float frequency, float persistence);
WORLDAPI_EXPORT void generatePerlinNoise2D(arma::Mat<double> &output, int offset, int octaves, float frequency, float persistence, const arma::Mat<double> & model);
WORLDAPI_EXPORT arma::Mat<double> generatePerlinNoise2D(int size, int offset, int octaves, float frequency, float persistence);

void generatePerlinOctave(arma::Mat<double> & output, int offset, float frequency);
void generatePerlinOctave(arma::Mat<double> & output, int offset, float frequency, const arma::Mat<double> & model);