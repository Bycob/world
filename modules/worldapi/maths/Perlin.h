/* Ce fichier contient des fonctions utiles pour générer du bruit de perlin.
Il se base sur la bibliothèque matricielle armadillo.*/
#pragma once

#include <worldapi/worldapidef.h>

#include <random>
#include <armadillo/armadillo>

namespace perlin {
    enum class WORLDAPI_EXPORT Direction {
        AXIS_X, AXIS_Y,
    };
}

class WORLDAPI_EXPORT Perlin {
public :
    Perlin();
    Perlin(long seed);

    void generatePerlinNoise2D(arma::Mat<double> &output,
                               int offset,
                               int octaves,
                               float frequency,
                               float persistence,
                               bool repeatable = false);

    arma::Mat<double> generatePerlinNoise2D(int size,
                                            int offset,
                                            int octaves,
                                            float frequency,
                                            float persistence,
                                            bool repeatable = false);

    /** Cette fonction permet de modifier les bords des deux matrices passées
    en paramètres de sorte qu'elle s'assemble parfaitement selon la direction
    souhaitée. La jointure est effectuée par un bruit de perlin, dont les
    caractéristiques sont détaillées en paramètres.
    @param direction La direction de jointure. Par exemple, direction = AXIS_X
     signifie que lorsqu'on va vers les x croissants de mat1, on finit par
     arriver dans mat2.
    @param joinableSides Ce paramètre indique que la grille doit rester jointive
     avec les grilles adjacentes dans l'autre direction. Ainsi, si on joint deux
     couples de grilles dans une direction, et qu'on souhaite les joindre également
     dans l'autre direction, on peut mettre ce paramètre à true pour conserver
     la jointure. */
    void join(arma::Mat<double> &matDown,
              arma::Mat<double> &matUp,
              const perlin::Direction &direction,
              int octaves,
              float frequency,
              float persistence,
              bool joinableSides = false);

private :
    std::mt19937 _rng;
    std::uniform_real_distribution<double> _random;

    void generatePerlinOctave(arma::Mat<double> &output,
                              int offset,
                              float frequency,
                              bool repeatable);
};