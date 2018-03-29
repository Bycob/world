#include "Perlin.h"

#include <armadillo/armadillo>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <functional>

#include "MathsHelper.h"
#include "Interpolation.h"

using namespace arma;

namespace world {

    using namespace perlin;

    Perlin::modifier Perlin::DEFAULT_MODIFIER = [&](double, double, double val) {
        return val;
    };

    std::vector<double> getCoefs(int octaves, double persistence) {
        std::vector<double> coefs;

        double persistenceSum = 0;
        for (int i = 1; i <= octaves; i++) {
            double p = pow(persistence, i);
            persistenceSum += p;
            coefs.push_back(p);
        }

        for (int i = 0; i < octaves; i++) {
            coefs[i] /= persistenceSum;
        }

        return coefs;
    }

    Perlin::Perlin() : Perlin::Perlin((long) time(NULL)) {

    }

    Perlin::Perlin(long seed) : _rng(seed), _random(0.0, 1.0) {

    }

    void Perlin::growBuffer(arma::uword size) {
        if (_buffer.n_rows < size) {
            _buffer = arma::mat(size, size);
        }
    }

    void Perlin::generatePerlinOctave(Mat<double> &output,
                                      int offset,
                                      double frequency,
                                      bool repeatable,
                                      const modifier &sourceModifier) {

        uword fi = (uword) ceil(frequency);
        growBuffer(fi + 1);

        // Fill buffer
        for (int x = 0; x <= fi; x++) {
            for (int y = 0; y <= fi; y++) {
                double val = _random(_rng);

                if (repeatable) {
                    if (x == fi) {
                        val = _buffer(0, y);
                    } else if (y == fi) {
                        val = _buffer(x, 0);
                    }
                }

                _buffer(x, y) = sourceModifier((double) x / fi, (double) y / fi, val);
            }
        }

        // Build octave
        for (int x = 0; x < output.n_rows; x++) {
            for (int y = 0; y < output.n_cols; y++) {
                double xd = (double) x / output.n_rows;
                double yd = (double) y / output.n_cols;

                // Bounds
                double f1 = frequency;
                int borneX1 = (int) (xd * f1);
                int borneY1 = (int) (yd * f1);
                int borneX2 = min(borneX1 + 1, (int) fi);
                int borneY2 = min(borneY1 + 1, (int) fi);

                // Interpolation
                double v1 = interpolateCosine(borneX1 / f1, _buffer(borneX1, borneY1),
                                              borneX2 / f1, _buffer(borneX2, borneY1),
                                              xd);

                double v2 = interpolateCosine(borneX1 / f1, _buffer(borneX1, borneY2),
                                              borneX2 / f1, _buffer(borneX2, borneY2),
                                              xd);

                output(x, y) = interpolateCosine(borneY1 / f1, v1, borneY2 / f1, v2, yd);
            }
        }
    }

    void Perlin::generatePerlinNoise2D(arma::Mat<double> &output, int offset, int octaves, double frequency,
                                       double persistence, bool repeatable) {
        generatePerlinNoise2D(output, offset, octaves, frequency, persistence, repeatable, DEFAULT_MODIFIER);
    }

    void Perlin::generatePerlinNoise2D(Mat<double> &output,
                                       int offset,
                                       int octaveCount,
                                       double frequency,
                                       double persistence,
                                       bool repeatable,
                                       const modifier &sourceModifier) {

        //Détermination de la taille de la matrice
        const uword size = std::min(output.n_rows, output.n_cols);

        output.fill(0);

        std::vector<double> coefs = getCoefs(octaveCount, persistence);

        Mat<double> octave(size, size);
        for (int i = 1; i <= octaveCount; i++) {
            generatePerlinOctave(octave, offset * i, frequency * (float) pow(2, i - 1), repeatable, sourceModifier);
            output += octave * coefs[i - 1];
        }
    }

    Mat<double> Perlin::generatePerlinNoise2D(int size,
                                              int offset,
                                              int octaves,
                                              double frequency,
                                              double persistence,
                                              bool repeatable) {

        Mat<double> result((uword) size, (uword) size);
        generatePerlinNoise2D(result, offset, octaves, frequency, persistence, repeatable);
        return result;
    }


// ICI LE MUSEE
// HERE IS THE MUSEUM

    void Perlin::join(arma::Mat<double> &mat1,
                      arma::Mat<double> &mat2,
                      const Direction &direction,
                      int octaves,
                      double frequency,
                      double persistence,
                      bool joinableSides) {

        // Vérification des dimensions de mat1 et mat2
        int length1, length2, depth1, depth2;
        if (direction == Direction::AXIS_Y) {
            length1 = (int) mat1.n_cols;
            length2 = (int) mat2.n_cols;
            depth1 = (int) mat1.n_rows;
            depth2 = (int) mat2.n_rows;
        } else {
            length1 = (int) mat1.n_rows;
            length2 = (int) mat2.n_rows;
            depth1 = (int) mat1.n_cols;
            depth2 = (int) mat2.n_cols;
        }

        // TODO définir le comportement lorsque length2 /= length1
        int length = min(length1, length2);
        int depth = min(depth1, depth2);
        int period = max(1, (int) ((depth - 1) / frequency));

        int joinDepth1 = -(depth1 % period) - period;
        int joinDepth2 = period + 1;
        int joinDepth = joinDepth2 - joinDepth1;
        int joinOffset = -joinDepth1;

        // Permet d'abstraire les directions.
        // les x sont dans le sens de la longueur des deux matrices à joindre
        // les y représentent l'éloignement à la jointure.
        // y < joinOffset -> mat1, y > joinOffset -> mat2
        std::function<double &(int, int)> at;

        switch (direction) {
            case Direction::AXIS_Y:
                at = [&mat1, &mat2, &joinOffset](int x, int y) -> double & {
                    y -= joinOffset;
                    if (y < 0) {
                        return mat1(x, mat1.n_cols + y);
                    } else {
                        return mat2(x, y);
                    }
                };
                break;
            case Direction::AXIS_X:
                at = [&mat1, &mat2, &joinOffset](int x, int y) -> double & {
                    y -= joinOffset;
                    if (y < 0) {
                        return mat1(mat1.n_rows + y, x);
                    } else {
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

            // Génération des points aléatoires
            for (int y = joinDepth - period - 1; y >= period; y -= period) {
                for (int x = 0; x < length; x += period) {

                    // Permet à ce que la dernière période s'étende sur toute la place disponible
                    if (x >= length - period) {
                        x = length - 1;
                    }

                    // Si le décor doit être juxtaposable sur le côté,
                    // on fait un schéma plus simple pour les bords
                    if (joinableSides && (x == length - 1 || x == 0)) {
                        octave(x, y) = interpolateCosine(0, at(x, 0), joinDepth - 1, at(x, joinDepth - 1), y);
                        //if (x == length - 1) std::cout << "y=" << y << " : " << octave(x, y) << std::endl;
                    } else {
                        octave(x, y) = _random(_rng);
                    }
                }
            }

            // Interpolation entre les points
            for (int x = 0; x < length; x++) {
                for (int y = 0; y < joinDepth; y++) {
                    int minusY = joinDepth - y - 1;

                    if (y != 0 && y != joinDepth - 1 &&
                        (x % period != 0 ||
                         minusY % period != 0 || y < period || minusY <= period)) {

                        int borneX1, borneX2, borneY1, borneY2;

                        // Détermination des bornes en y
                        borneY1 = joinDepth - (minusY / period) * period - 1;
                        if (borneY1 < period) borneY1 += period;
                        borneY2 = borneY1 - period;
                        if (borneY2 < period) borneY2 = 0;

                        // Détermination des bornes en x
                        borneX1 = (x / period) * period;

                        if (joinableSides && borneX1 >= length - period) {
                            borneX1 -= period;
                            borneX2 = length - 1;
                        } else {
                            borneX2 = borneX1 + period;
                            if (borneX2 >= length) borneX2 = length - 1;
                        }

                        // Interpolation en x
                        const std::function<double(int)> interpolateX = [&](int borneY) -> double {
                            if (borneY == 0 || borneY == joinDepth - 1) {
                                return at(x, borneY);
                            } else {
                                //if (joinableSides && x == length - 1) std::cout << "at " << borneX2 << " there is " << octave(borneX2, borneY) << std::endl;
                                return interpolateCosine(borneX1, octave(borneX1, borneY), borneX2,
                                                         octave(borneX2, borneY), x);
                            }
                        };

                        double v1 = interpolateX(borneY1);
                        double v2 = interpolateX(borneY2);

                        // Interpolation en y
                        octave(x, y) = interpolateCosine(borneY2, v2, borneY1, v1, y);

                        //if (joinableSides && x == length - 1 ) std::cout  << "with " << y << " we have " << borneX1 << " , " << borneX2 << " and " << borneY1 << " , " << borneY2 << " : " << octave(x, y) <<  std::endl;
                    }
                }
            }

            // Ajout de l'octave au joint
            double coef = coefs[i - 1];
            for (int x = 0; x < length; x++) {
                for (int y = 1; y < joinDepth - 1; y++) {
                    if (i == 1) {
                        at(x, y) = coef * octave(x, y);
                    } else {
                        at(x, y) += coef * octave(x, y);
                    }
                }
            }

            // Incrémentation de la période
            period = (int) ceil(period / 2.0);
        }
    }

}