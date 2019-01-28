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

std::vector<double> getCoefs(int octaves, double persistence, bool normalize) {
    std::vector<double> coefs;

    double persistenceSum = 0;
    for (int i = 0; i < octaves; i++) {
        double p = pow(persistence, i);
        persistenceSum += p;
        coefs.push_back(p);
    }

    if (normalize) {
        for (int i = 0; i < octaves; i++) {
            coefs[i] /= persistenceSum;
        }
    }

    return coefs;
}

int getOffset(int offset, int octave, const PerlinInfo &info) {
    if (octave >= info.reference) {
        return offset * powi(2, octave - info.reference);
    } else {
        return static_cast<int>(
            floor(offset * powi(2., octave - info.reference)));
    }
}

double getOffsetf(int offset, int octave, const PerlinInfo &info) {
    double rawOffset = offset * powi(2., octave - info.reference);
    return rawOffset - floor(rawOffset);
}

Perlin::Perlin() : Perlin::Perlin(static_cast<long>(time(NULL))) {}

Perlin::Perlin(long seed) : _rng(seed) {
    for (u32 i = 0; i < 256; ++i) {
        _hash[i] = static_cast<u8>(i);
    }

    std::shuffle(std::begin(_hash), std::begin(_hash) + 256, _rng);

    for (u32 i = 0; i < 256; ++i) {
        _hash[i + 256] = _hash[i];
    }
}

void Perlin::setNormalize(bool normalize) { _normalize = normalize; }

std::vector<u8> Perlin::getHash() const {
    return std::vector<u8>(_hash, _hash + 512);
}

double Perlin::getMaxPossibleValue(const PerlinInfo &info) {
    if (_normalize)
        return 1;

    double persistenceSum = 0;

    for (int i = 0; i < info.octaves; i++) {
        double p = pow(info.persistence, i);
        persistenceSum += p;
    }

    return persistenceSum;
}

void Perlin::growBuffer(arma::uword size) {
    if (_buffer.n_rows < size) {
        _buffer = arma::mat(size, size);
    }
}

void Perlin::fillBuffer(int octave, const PerlinInfo &info,
                        const modifier &sourceModifier) {

    double localFreq = info.frequency * powi(2., octave);
    uword fi = static_cast<uword>(ceil(localFreq));
    growBuffer(fi + 1);

    int offX = getOffset(info.offsetX, octave, info);
    int offY = getOffset(info.offsetY, octave, info);

    // Fill buffer
    for (int x = 0; x <= fi; x++) {
        for (int y = 0; y <= fi; y++) {
            u32 px = static_cast<u32>(x + offX) & 0xFFu;
            u32 py = static_cast<u32>(y + offY) & 0xFFu;
            double val = _hash[px + _hash[py + _hash[octave]]] / 255.;

            if (info.repeatable) {
                if (x == fi) {
                    val = _buffer(0, y);
                } else if (y == fi) {
                    val = _buffer(x, 0);
                }
            }

            _buffer(x, y) = sourceModifier((double)x / fi, (double)y / fi, val);
        }
    }
}

void Perlin::generatePerlinOctave(arma::Mat<double> &output, int octave,
                                  const PerlinInfo &info,
                                  const modifier &sourceModifier) {

    fillBuffer(octave, info, sourceModifier);

    const double f = info.frequency * powi(2., octave - info.reference);
    const double offXf = getOffsetf(info.offsetX, octave, info);
    const double offYf = getOffsetf(info.offsetY, octave, info);

    // Build octave
    for (int x = 0; x < output.n_rows; x++) {
        for (int y = 0; y < output.n_cols; y++) {
            double xd = f * x / (output.n_rows - 1) + offXf;
            double yd = f * y / (output.n_cols - 1) + offYf;

            // Bounds
            int borneX1 = static_cast<int>(floor(xd));
            int borneY1 = static_cast<int>(floor(yd));
            int borneX2 = static_cast<int>(ceil(xd));
            int borneY2 = static_cast<int>(ceil(yd));

            // Interpolation
            double v1 = Interpolation::interpolateCosine(
                borneX1, _buffer(borneX1, borneY1), borneX2,
                _buffer(borneX2, borneY1), xd);

            double v2 = Interpolation::interpolateCosine(
                borneX1, _buffer(borneX1, borneY2), borneX2,
                _buffer(borneX2, borneY2), xd);

            output(x, y) =
                Interpolation::interpolateCosine(borneY1, v1, borneY2, v2, yd);
        }
    }
}

void Perlin::generatePerlinNoise2D(arma::Mat<double> &output,
                                   const PerlinInfo &info) {
    generatePerlinNoise2D(output, info, DEFAULT_MODIFIER);
}

void Perlin::generatePerlinNoise2D(Mat<double> &output, const PerlinInfo &info,
                                   const modifier &sourceModifier) {

    // Détermination de la taille de la matrice
    const uword size = std::min(output.n_rows, output.n_cols);

    output.fill(0);

    std::vector<double> coefs =
        getCoefs(info.octaves, info.persistence, _normalize);

    Mat<double> octave(size, size);
    for (int i = 0; i < info.octaves; i++) {
        generatePerlinOctave(octave, i, info, sourceModifier);
        output += octave * coefs[i];
    }
}

Mat<double> Perlin::generatePerlinNoise2D(int size, const PerlinInfo &info) {

    Mat<double> result((uword)size, (uword)size);
    generatePerlinNoise2D(result, info);
    return result;
}

} // namespace world