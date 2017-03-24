/* Ce fichier contient des fonctions utiles pour g�n�rer du bruit de perlin.
Il se base sur la biblioth�que matricielle armadillo.*/
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

	enum class WORLDAPI_EXPORT Direction {
		AXIS_X, AXIS_Y,
	};

	/** Cette fonction permet de modifier les bords des deux matrices pass�es
	en param�tres de sorte qu'elle s'assemble parfaitement selon la direction
	souhait�e. La jointure est effectu�e par un bruit de perlin, dont les
	caract�ristiques sont d�taill�es en param�tres.
	@param direction La direction de jointure. Par exemple, direction = AXIS_X
	signifie que lorsqu'on va vers les x croissants de mat1, on finit par
	arriver dans mat2.*/
	WORLDAPI_EXPORT void join(arma::Mat<double> &matDown,
		                      arma::Mat<double> &matUp,
		                      const Direction & direction,
		                      int octaves,
		                      float frequency,
		                      float persistence);
}