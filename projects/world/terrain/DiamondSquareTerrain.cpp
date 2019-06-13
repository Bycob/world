#include "DiamondSquareTerrain.h"

#include <iostream>

namespace world {

DiamondSquareTerrain::DiamondSquareTerrain(double jitter)
        : _rng(time(NULL)), _jitter(-jitter / 2, jitter / 2) {}

void DiamondSquareTerrain::processTile(ITileContext &context) {
    int lod = context.getParentCount();
    Terrain &terrain = context.getTerrain();

    if (lod == 0) {
        processTerrain(terrain);
    } else {
        copyParent(context.getParent().value(), terrain);
        compute(terrain, 1);
    }
}

int findMaxLevel(int terrainRes) {
    int level = 1;
    while (terrainRes > powi(2, level) + 1) {
        level++;
    }
    return level;
}


void DiamondSquareTerrain::processTerrain(Terrain &terrain) {
    int maxLevel = findMaxLevel(terrain.getResolution());
    // TODO warn user when terrain is not 2^n + 1

    init(terrain);

    for (int i = maxLevel; i >= 1; --i) {
        compute(terrain, i);
    }
}

void DiamondSquareTerrain::init(Terrain &terrain) {
    std::uniform_real_distribution<double> dist(0, 1);
    int r = terrain.getResolution() - 1;
    terrain(0, 0) = dist(_rng);
    terrain(r, 0) = dist(_rng);
    terrain(0, r) = dist(_rng);
    terrain(r, r) = dist(_rng);
}

double DiamondSquareTerrain::value(double h1, double h2) {
    return h1 + (h2 - h1) * (0.5 + _jitter(_rng));
}

// level 1 (minimum) -> square size of 2.
// level n -> square size of 2^n

void DiamondSquareTerrain::compute(Terrain &terrain, int level) {
    int res = terrain.getResolution() - 1;
    int n = powi(2, level);
    int hn = n / 2;

    for (int y = 0; y < res; y += n) {
        for (int x = 0; x < res; x += n) {
            // square
            if (x == 0) {
                terrain(x, y + hn) = value(terrain(x, y), terrain(x, y + n));
            }
            if (y == 0) {
                terrain(x + hn, y) = value(terrain(x, y), terrain(x + n, y));
            }

            terrain(x + hn, y + n) =
                value(terrain(x, y + n), terrain(x + n, y + n));
            terrain(x + n, y + hn) =
                value(terrain(x + n, y), terrain(x + n, y + n));

            // diamond
            double v1 = value(terrain(x, y + hn), terrain(x + n, y + hn));
            double v2 = value(terrain(x + hn, y), terrain(x + hn, y + n));
            terrain(x + hn, y + hn) = (v1 + v2) / 2;
        }
    }
}

void DiamondSquareTerrain::copyParent(const Terrain &parent, Terrain &terrain) {
    // We assume child resolution is 2^n + 1 and parent one is 2^(n-1) +1
    int pres = parent.getResolution();

    for (int y = 0; y < pres; ++y) {
        for (int x = 0; x < pres; ++x) {
            terrain(x * 2, y * 2) = parent(x, y);
        }
    }
}

} // namespace world
