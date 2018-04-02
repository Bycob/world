#pragma once

#include "core/WorldConfig.h"

#include <functional>
#include <random>

#include <armadillo/armadillo>

namespace perlin {
    enum class WORLDAPI_EXPORT Direction {
        AXIS_X, AXIS_Y,
    };
}

namespace world {

    class WORLDAPI_EXPORT Perlin {
    public :
        typedef std::function<double(double, double, double)> modifier;

        static modifier DEFAULT_MODIFIER;

        Perlin();

        Perlin(long seed);

        void generatePerlinNoise2D(arma::Mat<double> &output,
                                   int offset,
                                   int octaves,
                                   double frequency,
                                   double persistence,
                                   bool repeatable = false);

        void generatePerlinNoise2D(arma::Mat<double> &output,
                                   int offset,
                                   int octaves,
                                   double frequency,
                                   double persistence,
                                   bool repeatable,
                                   const modifier &sourceModifier);

        arma::Mat<double> generatePerlinNoise2D(int size,
                                                int offset,
                                                int octaves,
                                                double frequency,
                                                double persistence,
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
         la jointure.
        @deprecated parce qu'extremement moche*/
        void join(arma::Mat<double> &matDown,
                  arma::Mat<double> &matUp,
                  const perlin::Direction &direction,
                  int octaves,
                  double frequency,
                  double persistence,
                  bool joinableSides = false);

    private :
        std::mt19937 _rng;
        std::uniform_real_distribution<double> _random;

        // Buffer for perlin points
        arma::mat _buffer;

        void growBuffer(arma::uword size);

        void generatePerlinOctave(arma::Mat<double> &output,
                                  int offset,
                                  double frequency,
                                  bool repeatable,
                                  const modifier &sourceModifier);
    };
}
