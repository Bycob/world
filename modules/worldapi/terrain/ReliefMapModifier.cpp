#include "ReliefMapModifier.h"

#include <utility>
#include <list>
#include <tuple>

#include "assets/Image.h"
#include "math/MathsHelper.h"
#include "math/Interpolation.h"
#include "TerrainOps.h"

using namespace arma;

namespace world {

// -----
ReliefMapModifier::ReliefMapModifier() : _rng(static_cast<u32>(time(NULL))) {}

void ReliefMapModifier::setMapResolution(int mapres) {
    _mapResolution = mapres;
}

void ReliefMapModifier::processTerrain(Terrain &terrain) {
    // Terrain
    int size = terrain.getResolution();

    // Map
    // TODO plut�t qu'un const cast faire une m�thode priv�e
    std::pair<Terrain, Terrain> &reliefMap =
        const_cast<std::pair<Terrain, Terrain> &>(obtainMap(0, 0));
    Terrain &heightMap = reliefMap.first;
    Terrain &diffMap = reliefMap.second;

    // Setup variables
    const vec3d terrainDims = terrain.getBoundingBox().getDimensions();
    const vec3d terrainPos = terrain.getBoundingBox().getLowerBound();
    const double mapSize = heightMap.getResolution() * _mapPointSize;
    const double ratio = terrainDims.x / mapSize;
    const double mapOx = 0.5 + terrainPos.x / mapSize;
    const double mapOy = 0.5 + terrainPos.y / mapSize;

    const double offsetCoef = 0.5;
    const double diffCoef = 1 - offsetCoef;

    // std::cout << "apply to " << mapOx << ", " << mapOy << std::endl;

    arma::mat bufferOffset(size, size);
    arma::mat bufferDiff(size, size);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            double mapX = mapOx + ((double)x / (size - 1)) * ratio;
            double mapY = mapOy + ((double)y / (size - 1)) * ratio;

            double offset = heightMap.getInterpolatedHeight(
                                mapX, mapY, Interpolation::COSINE) *
                            offsetCoef;
            double diff = diffMap.getInterpolatedHeight(mapX, mapY,
                                                        Interpolation::COSINE) *
                          diffCoef;

            bufferOffset(x, y) = offset;
            bufferDiff(x, y) = diff;

            // to unapply
            // bufferOffset(x, y) = -offset;
            // bufferDiff(x, y) = diff > std::numeric_limits<double>::epsilon()
            // ? 1 / diff : 0;
        }
    }

    TerrainOps::multiply(terrain, bufferDiff);
    TerrainOps::applyOffset(terrain, bufferOffset);

    // To unapply
    // TerrainOps::applyOffset(terrain, bufferOffset);
    // TerrainOps::multiply(terrain, bufferDiff);
}

void ReliefMapModifier::processTile(ITileContext &context) {
    processTerrain(context.getTerrain());
}

const std::pair<Terrain, Terrain> &ReliefMapModifier::obtainMap(int x, int y) {
    auto it = _reliefMap.find({x, y});

    if (it == _reliefMap.end()) {
        it = _reliefMap
                 .emplace(vec2i{x, y},
                          std::make_pair<Terrain, Terrain>(
                              Terrain(_mapResolution), Terrain(_mapResolution)))
                 .first;
        generate(it->second.first, it->second.second);
    }
    return it->second;
}

// -----
const double CustomWorldRMModifier::PIXEL_UNIT = 10;

CustomWorldRMModifier::CustomWorldRMModifier(double biomeDensity,
                                             int limitBrightness)
        : _biomeDensity(biomeDensity), _limitBrightness(limitBrightness),
          _offsetLaw(ReliefMapParams::CustomWorldElevation(2. / 3.)),
          _diffLaw(ReliefMapParams::CustomWorldDifferential(2. / 3.)) {}

void CustomWorldRMModifier::setBiomeDensity(float biomeDensity) {
    _biomeDensity = biomeDensity;
}

void CustomWorldRMModifier::setLimitBrightness(int p) { _limitBrightness = p; }

void CustomWorldRMModifier::setDifferentialLaw(const AltDiffParam &law) {
    _diffLaw = law;
}

void CustomWorldRMModifier::generate(Terrain &height, Terrain &heightDiff) {
    // Nombre de biomes � g�n�rer.
    int size = height.getResolution() * height.getResolution();
    int biomeCount =
        (int)(_biomeDensity * (double)size / (PIXEL_UNIT * PIXEL_UNIT));


    // -> Cr�ation de la grille pour le placement des points de mani�re
    // al�atoire, mais avec une distance minimum.

    // Calcul des dimensions de la grille
    double minDistance = PIXEL_UNIT / 2.0 * sqrt(_biomeDensity);

    int sliceCount =
        max<int>((int)((float)height.getResolution() / minDistance), 1);
    float sliceSize = (float)height.getResolution() / (float)sliceCount;

    // the terrains are squared so there are as much slices as there are cases
    // per slice
    int caseCount = sliceCount;
    float caseSize = sliceSize;

    // Pr�paration de la grille
    typedef std::pair<vec2d, vec2d> point; // pour plus de lisibilit�
    std::vector<std::vector<point>> pointsMap;
    pointsMap.reserve(sliceCount);
    std::vector<std::pair<int, int>> grid;
    grid.reserve(sliceCount * caseCount);

    for (int x = 0; x < sliceCount; x++) {
        pointsMap.emplace_back();

        std::vector<point> &slice = pointsMap[x];
        slice.reserve(caseCount);

        for (int y = 0; y < caseCount; y++) {
            grid.emplace_back(x, y);
            slice.emplace_back(vec2d(-1, -1), vec2d(0, 0));
        }
    }

    // G�n�ration des points
    std::uniform_real_distribution<double> rand(0.0, 1.0);

    for (int i = 0; i < biomeCount;
         i++) { // TODO dans les cas limites la grille peut se vider totalement
        // G�n�ration des coordonn�es des points
        int randIndex = (int)(rand(_rng) * grid.size());
        std::pair<int, int> randPoint = grid.at(randIndex);
        grid.erase(grid.begin() + randIndex);

        int x = randPoint.first;
        int y = randPoint.second;

        // Calcul des limites dans lesquelles on peut avoir un point
        double limNegX = 0;
        double limPosX = sliceSize;
        double limNegY = 0;
        double limPosY = caseSize;

        if (x > 0) {
            auto negXCase = pointsMap[x - 1][y];

            if (negXCase.first.x >= 0) {
                limNegX = negXCase.first.x;
            }
        }
        if (x < sliceCount - 1) {
            auto posXCase = pointsMap[x + 1][y];

            if (posXCase.first.x >= 0) {
                limPosX = posXCase.first.x;
            }
        }
        if (y > 0) {
            auto negYCase = pointsMap[x][y - 1];

            if (negYCase.first.y >= 0) {
                limNegY = negYCase.first.y;
            }
        }
        if (y < caseCount - 1) {
            auto posYCase = pointsMap[x][y + 1];

            if (posYCase.first.y >= 0) {
                limPosY = posYCase.first.y;
            }
        }

        // � partir des limites on peut d�terminer la position random du point
        double randX = rand(_rng);
        double randY = rand(_rng);

        // TODO L'utilisateur n'a aucun contr�le sur le premier param�tre.
        double elevation = _offsetLaw();
        double diff = _diffLaw(elevation);

        pointsMap[x][y] = {
            vec2d(randX * (limPosX - limNegX) + limNegX + x * sliceSize,
                  randY * (limPosY - limNegY) + limNegY + y * caseSize),
            vec2d(elevation, diff)};
    }


    // -> Interpolation des valeurs des points pour reconstituer une map

    // Cr�ation des interpolateur
    IDWInterpolator<vec2d, vec2d> interpolator(_limitBrightness);

    // On pr�pare les donn�es � interpoler.
    for (auto &slice : pointsMap) {
        for (auto &pt : slice) {
            if (pt.first.x >= 0) {
                interpolator.addData(pt.first, pt.second);
            }
        }
    }

    // On remplit la grille � l'aide de l'interpolateur. And enjoy.
    for (int x = 0; x < height.getResolution(); x++) {
        for (int y = 0; y < height.getResolution(); y++) {
            vec2d pt(x + 0.5, y + 0.5);
            vec2d result = interpolator.getData(pt);
            height(x, y) = result.x;
            heightDiff(x, y) = result.y;
        }
    }
}
} // namespace world
