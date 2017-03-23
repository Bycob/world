#include "perlin.h"

#include <armadillo/armadillo>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "mathshelper.h"
#include "interpolation.h"

using namespace arma;
using namespace maths;

namespace perlin {
    //PRIVATE génère un nombre aléatoire entre 0 et 1
    double generateRandom() {
        return rand() / (double) RAND_MAX;
    }

    void generatePerlinOctave(Mat<double> &output,
                              int offset,
                              float frequency,
                              bool repeatable,
                              bool borders) {

        //Détermination de la taille de la matrice
        int size = (int) std::min(output.n_rows, output.n_cols);
        //marche le mieux avec des tailles de pattern de la forme 2^n + 1 et fréquence en 2^i
        int period = max((int) ((size - 1) / frequency), 1);

        // Implémentation des bordures et de la répétabilité
        int firstX = borders ? period : 0, firstY = firstX;
        int lastX = (borders || repeatable) ? size - period : size, lastY = lastX;

        int overflowX = period * ((size - 1) / period);
        if (repeatable) {
            overflowX = size;
        }
        else if (borders) {
            overflowX = size - 1;
        }
        int overflowY = overflowX;

        //Génération des valeurs aléatoires
        for (int x = firstX; x < lastX; x += period) {
            for (int y = firstY; y < lastY; y += period) {
                output(x, y) = generateRandom();
            }
        }

        //Interpolation
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {

                if (x % period != 0 || y % period != 0) {
                    //Calcul des bornes
                    int borneX1, borneX2, borneY1, borneY2;

                    int tileX = x / period;
                    int tileY = y / period;

                    borneX1 = tileX * period;
                    borneY1 = tileY * period;
                    borneX2 = borneX1 + period;
                    borneY2 = borneY1 + period;

                    if (borneX2 >= lastX) {
                        borneX2 = overflowX;
                    }
                    if (borneY2 >= lastY) {
                        borneY2 = overflowY;
                    }


                    //Interpolation
                    double v1 = interpolate(borneX1, output(borneX1, borneY1),
                                            borneX2, output(borneX2 % size, borneY1),
                                            x);

                    double v2 = interpolate(borneX1, output(borneX1, borneY2 % size),
                                            borneX2, output(borneX2 % size, borneY2 % size),
                                            x);

                    output(x, y) = interpolate(borneY1, v1, borneY2 % size, v2, y);
                }
            }
        }
    }

    void generatePerlinNoise2D(Mat<double> &output,
                               int offset,
                               int octaveCount,
                               float frequency,
                               float persistence,
                               bool repeatable,
                               bool borders) {

        //Initialisation du random avec une seed quelconque
        srand(time(NULL));

        //Détermination de la taille de la matrice
        const uword size = std::min(output.n_rows, output.n_cols);

        output.fill(0);

        double persistenceSum = 0;
        for (int i = 1; i <= octaveCount; i++) {
            persistenceSum += pow(persistence, i);
        }

        Mat<double> octave(size, size);
        for (int i = 1; i <= octaveCount; i++) {
            generatePerlinOctave(octave, offset * (i - 1), frequency * (float) pow(2, i - 1), repeatable, borders);
            output += octave * pow(persistence, i) / persistenceSum;
        }
    }

    Mat<double> generatePerlinNoise2D(int size,
                                      int offset,
                                      int octaves,
                                      float frequency,
                                      float persistence,
                                      bool repeatable) {

        Mat<double> result(size, size);
        generatePerlinNoise2D(result, offset, octaves, frequency, persistence, repeatable, false);
        return result;
    }
}
