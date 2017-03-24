#include "perlin.h"

#include <armadillo/armadillo>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <functional>

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
                              bool repeatable) {

        //Détermination de la taille de la matrice
        int size = (int) std::min(output.n_rows, output.n_cols);
        //marche le mieux avec des tailles de pattern de la forme 2^n + 1 et fréquence en 2^i
        int period = max((int) ((size - 1) / frequency), 1);

        // Implémentation de la répétabilité
        int firstX = 0, firstY = firstX;
        int lastX = repeatable ? size - period : size, lastY = lastX;

        int overflowX = period * ((size - 1) / period);
        if (repeatable) {
            overflowX = size;
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

                if (x % period != 0 || x >= lastX || 
					y % period != 0 || y >= lastY) {

                    //Calcul des bornes
                    int borneX1, borneX2, borneY1, borneY2;

                    int tileX = min(x, lastX) / period;
                    int tileY = min(y, lastY) / period;

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

                    output(x, y) = interpolate(borneY1, v1, borneY2, v2, y);
                }
            }
        }
    }

    void generatePerlinNoise2D(Mat<double> &output,
                               int offset,
                               int octaveCount,
                               float frequency,
                               float persistence,
                               bool repeatable) {

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
            generatePerlinOctave(octave, offset * (i - 1), frequency * (float) pow(2, i - 1), repeatable);
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
        generatePerlinNoise2D(result, offset, octaves, frequency, persistence, repeatable);
        return result;
    }

	void join(arma::Mat<double> &mat1,
		      arma::Mat<double> &mat2,
		      const Direction & direction,
		      int octaves,
		      float frequency,
		      float persistence) {

		// Vérification des dimensions de mat1 et mat2
		int length1, length2, depth1, depth2;
		if (direction == Direction::AXIS_Y) {
			length1 = mat1.n_cols;
			length2 = mat2.n_cols;
			depth1 = mat1.n_rows;
			depth2 = mat2.n_rows;
		}
		else {
			length1 = mat1.n_rows;
			length2 = mat2.n_rows;
			depth1 = mat1.n_cols;
			depth2 = mat2.n_cols;
		}

		// TODO définir le comportement lorsque length2 /= length1
		int length = min(length1, length2);
		int depth = min(depth1, depth2);

		// Permet d'abstraire les directions.
		// les x sont dans le sens de la longueur des deux matrices à joindre
		// les y représentent l'éloignement à la jointure.
		// y négatif -> mat1, y positifs -> mat2
		std::function<double & (int, int)> at;

		switch (direction) {
		case Direction::AXIS_Y:
			at = [&mat1, &mat2](int x, int y) -> double & {
				if (y < 0) {
					return mat1(x, mat1.n_cols + y);
				}
				else {
					return mat2(x, y);
				}
			};
			break;
		case Direction::AXIS_X:
			at = [&mat1, &mat2](int x, int y) -> double & {
				if (y < 0) {
					return mat1(mat1.n_rows + y, x);
				}
				else {
					return mat2(y, x);
				}
			};
			break;
		}

		// Perlin
		int period = (int)ceil(depth / frequency);
		int joinDepth1 = (depth1 / period - 1) * period - depth1;
		int joinDepth2 = period;

		for (int i = 0; i < octaves; i++) {
			// Génération des points
			for (int y = joinDepth2; y >= joinDepth1 + period; y -= period) {

			}

			// Interpolation entre les points

			// Ajout de l'octave au joint

			// Incrémentation de la période
			period = (int) ceil(period / 2.0);
		}
	}
}
