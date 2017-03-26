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
    //PRIVATE g�n�re un nombre al�atoire entre 0 et 1
    double generateRandom() {
        return rand() / (double) RAND_MAX;
    }

    std::vector<double> getCoefs(int octaves, double persistence) {
        std::vector<double> coefs;

        double persistenceSum = 0;
        for (int i = 1; i <= octaves; i++) {
            double p = pow(persistence, i);
            persistenceSum += p;
            coefs.push_back(p);
        }

        for (int i = 0 ; i < octaves ; i++) {
            coefs[i] /= persistenceSum;
        }

        return coefs;
    }

    void generatePerlinOctave(Mat<double> &output,
                              int offset,
                              float frequency,
                              bool repeatable) {

        //D�termination de la taille de la matrice
        int size = (int) std::min(output.n_rows, output.n_cols);
        //marche le mieux avec des tailles de pattern de la forme 2^n + 1 et fr�quence en 2^i
        int period = max((int) ((size - 1) / frequency), 1);

        // Impl�mentation de la r�p�tabilit�
        int firstX = 0, firstY = firstX;
        int lastX = repeatable ? size - period : size, lastY = lastX;

        int overflowX = period * ((size - 1) / period);
        if (repeatable) {
            overflowX = size;
        }
        int overflowY = overflowX;

        //G�n�ration des valeurs al�atoires
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
        // TODO on pr�f�rera les seeds
        srand((uint32_t) time(NULL));

        //D�termination de la taille de la matrice
        const uword size = std::min(output.n_rows, output.n_cols);

        output.fill(0);

        std::vector<double> coefs = getCoefs(octaveCount, persistence);

        Mat<double> octave(size, size);
        for (int i = 1; i <= octaveCount; i++) {
            generatePerlinOctave(octave, offset * (i - 1), frequency * (float) pow(2, i - 1), repeatable);
            output += octave * coefs[i -1];
        }
    }

    Mat<double> generatePerlinNoise2D(int size,
                                      int offset,
                                      int octaves,
                                      float frequency,
                                      float persistence,
                                      bool repeatable) {

        Mat<double> result((uword) size, (uword) size);
        generatePerlinNoise2D(result, offset, octaves, frequency, persistence, repeatable);
        return result;
    }

	void join(arma::Mat<double> &mat1,
		      arma::Mat<double> &mat2,
		      const Direction & direction,
		      int octaves,
		      float frequency,
		      float persistence) {

		// V�rification des dimensions de mat1 et mat2
		int length1, length2, depth1, depth2;
		if (direction == Direction::AXIS_Y) {
			length1 = (int) mat1.n_cols;
			length2 = (int) mat2.n_cols;
			depth1 = (int) mat1.n_rows;
			depth2 = (int) mat2.n_rows;
		}
		else {
			length1 = (int) mat1.n_rows;
			length2 = (int) mat2.n_rows;
			depth1 = (int) mat1.n_cols;
			depth2 = (int) mat2.n_cols;
		}

		// TODO d�finir le comportement lorsque length2 /= length1
		int length = min(length1, length2);
		int depth = min(depth1, depth2);
        int period = maths::max(1, (int) ((depth - 1) / frequency));

        int joinDepth1 = - (depth1 % period) - period;
        int joinDepth2 = period + 1;
        int joinDepth = joinDepth2 - joinDepth1;
        int joinOffset = - joinDepth1;

		// Permet d'abstraire les directions.
		// les x sont dans le sens de la longueur des deux matrices � joindre
		// les y repr�sentent l'�loignement � la jointure.
		// y < joinOffset -> mat1, y > joinOffset -> mat2
		std::function<double & (int, int)> at;

		switch (direction) {
		case Direction::AXIS_Y:
			at = [&mat1, &mat2, &joinOffset](int x, int y) -> double & {
                y -= joinOffset;
				if (y < 0) {
					return mat1(x, mat1.n_cols + y);
				}
				else {
					return mat2(x, y);
				}
			};
			break;
		case Direction::AXIS_X:
			at = [&mat1, &mat2, &joinOffset](int x, int y) -> double & {
                y -= joinOffset;
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

        std::vector<double> coefs = getCoefs(octaves, persistence);
        Mat<double> join((uword) length, (uword) joinDepth);

		for (int i = 1; i <= octaves; i++) {
            Mat<double> octave((uword) length, (uword) joinDepth);

			// G�n�ration des points
			for (int y = joinDepth - period - 1; y >= period; y -= period) {
                for (int x = 0; x < length; x += period) {
                    octave(x, y) = generateRandom();
                }
			}

			// Interpolation entre les points
            for (int x = 0 ; x < length ; x++) {
                for (int y = 0 ; y < joinDepth ; y++) {
                    int minusY = joinDepth - y - 1;

                    if (y != 0 && y != joinDepth - 1 &&
                        (x % period != 0 ||
                        minusY % period != 0 || y < period || minusY <= period)) {

                        // D�termination des bornes en y
                        int borneY1 = joinDepth - (minusY / period) * period - 1;
                        if (borneY1 < period) borneY1 += period;
                        int borneY2 = borneY1 - period;
                        if (borneY2 < period) borneY2 = 0;

                        // D�termination des bornes en x
                        int borneX1 = (x / period) * period;
                        int borneX2 = borneX1 + period;
                        if (borneX2 >= length) borneX2 = borneX1;

                        // Interpolation en x
                        const std::function<double (int)> interpolateX = [&](int borneY) -> double {
                            if (borneY == 0 || borneY == joinDepth - 1) {
                                return at(x, borneY);
                            }
                            else {
                                return interpolate(borneX1, octave(borneX1, borneY), borneX2, octave(borneX2, borneY), x);
                            }
                        };

                        double v1 = interpolateX(borneY1);
                        double v2 = interpolateX(borneY2);

                        // Interpolation en y
                        octave(x, y) = interpolate(borneY2, v2, borneY1, v1, y);
                    }
                }
            }

			// Ajout de l'octave au joint
            double coef = coefs[i - 1];
            for (int x = 0 ; x < length ; x++) {
                for (int y = 1 ; y < joinDepth - 1; y++) {
                    if (i == 1) {
                        at(x, y) = coef * octave(x, y);
                    }
                    else {
                        at(x, y) += coef * octave(x, y);
                    }
                }
            }

			// Incr�mentation de la p�riode
			period = (int) ceil(period / 2.0);
		}
	}
}
