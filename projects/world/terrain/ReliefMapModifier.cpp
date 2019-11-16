#include "ReliefMapModifier.h"

#include <utility>
#include <list>
#include <tuple>

#include "world/assets/Image.h"
#include "world/math/MathsHelper.h"
#include "world/math/Interpolation.h"
#include "TerrainOps.h"

using namespace arma;

namespace world {

// -----
ReliefMapModifier::ReliefMapModifier(double width, int resolution)
        : _rng(static_cast<u32>(time(NULL))),
          _tileSystem(0, vec3i{resolution, resolution, 0},
                      vec3d{width, width, 0}) {}

void ReliefMapModifier::setMapResolution(int mapres) {
    _tileSystem._bufferRes = {mapres, mapres, 0};
}

void ReliefMapModifier::processTerrain(Terrain &terrain) {
    // Terrain
    int size = terrain.getResolution();

    // Map
    ReliefMapEntry &reliefMap = provideMap(0, 0);
    Terrain &heightMap = reliefMap._height;
    Terrain &diffMap = reliefMap._diff;

    // Setup variables
    const vec3d terrainDims = terrain.getBoundingBox().getDimensions();
    const vec3d terrainPos = terrain.getBoundingBox().getLowerBound();
    const double mapSize = _tileSystem._baseSize.x;
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

            double offset = heightMap.getCubicHeight(mapX, mapY) * offsetCoef;
            double diff = diffMap.getCubicHeight(mapX, mapY) * diffCoef;

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
    processTerrain(context.getTile().terrain());
}

const ReliefMapEntry &ReliefMapModifier::obtainMap(int x, int y) {
    return provideMap(x, y);
}

void ReliefMapModifier::setRegion(const vec2d &center, double radius,
                                  double curvature, double height,
                                  double diff) {

    const vec3d mapOffset = _tileSystem._baseSize / 2;
    const double mapRes = _tileSystem._bufferRes.x;
    const vec3d coords = vec3d(center) + mapOffset;
    const vec3d hsize{radius, radius, 0};
    const vec3i lmin =
        (_tileSystem.getLocalCoordinates(coords - hsize, 0) * (mapRes - 1))
            .floor();
    const vec3i lmax =
        (_tileSystem.getLocalCoordinates(coords + hsize, 0) * (mapRes - 1))
            .ceil();

    auto &map = provideMap(0, 0);

    const auto interp = [curvature](double x) { return pow(x, curvature); };

    for (int x = lmin.x; x < lmax.x; ++x) {
        for (int y = lmin.y; y < lmax.y; ++y) {
            double prevHeight = map._height(x, y);
            double prevDiff = map._diff(x, y);

            const vec2d gc = vec2d(_tileSystem.getGlobalCoordinates(
                {{}, 0}, vec3d(x, y, 0) / (mapRes - 1)));
            double d = center.length(gc - vec2d(mapOffset));

            map._height(x, y) = Interpolation::interpolate(
                0, height, radius, prevHeight, d, interp);
            map._diff(x, y) = Interpolation::interpolate(0, diff, radius,
                                                         prevDiff, d, interp);
        }
    }
}

ReliefMapEntry &ReliefMapModifier::provideMap(int x, int y) {
    int resolution = _tileSystem._bufferRes.x;

    return _reliefMap.getOrCreateCallback(
        TileCoordinates({x, y, 0}, 0),
        [&](ReliefMapEntry &elem) { generate(elem._height, elem._diff); },
        resolution);
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
    // Nombre de biomes à générer.
    int size = height.getResolution() * height.getResolution();
    int biomeCount =
        (int)(_biomeDensity * (double)size / (PIXEL_UNIT * PIXEL_UNIT));


    // -> Création de la grille pour le placement des points de manière
    // aléatoire, mais avec une distance minimum.

    // Calcul des dimensions de la grille
    double minDistance = PIXEL_UNIT / 2.0 * sqrt(_biomeDensity);

    int sliceCount =
        max<int>((int)((float)height.getResolution() / minDistance), 1);
    float sliceSize = (float)height.getResolution() / (float)sliceCount;

    // the terrains are squared so there are as much slices as there are cases
    // per slice
    int caseCount = sliceCount;
    float caseSize = sliceSize;

    // Préparation de la grille
    typedef std::pair<vec2d, vec2d> point; // pour plus de lisibilité
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

    // Génération des points
    std::uniform_real_distribution<double> rand(0.0, 1.0);

    for (int i = 0; i < biomeCount; i++) { // TODO dans les cas limites la
                                           // grille peut se vider totalement
        // Génération des coordonnées des points
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

        // à partir des limites on peut déterminer la position random du point
        double randX = rand(_rng);
        double randY = rand(_rng);

        // TODO L'utilisateur n'a aucun contrôle sur le premier paramètre.
        double elevation = _offsetLaw();
        double diff = _diffLaw(elevation);

        pointsMap[x][y] = {
            vec2d(randX * (limPosX - limNegX) + limNegX + x * sliceSize,
                  randY * (limPosY - limNegY) + limNegY + y * caseSize),
            vec2d(elevation, diff)};
    }


    // -> Interpolation des valeurs des points pour reconstituer une map

    // Création des interpolateur
    IDWInterpolator<vec2d, vec2d> interpolator(_limitBrightness);

    // On prépare les données à interpoler.
    for (auto &slice : pointsMap) {
        for (auto &pt : slice) {
            if (pt.first.x >= 0) {
                interpolator.addData(pt.first, pt.second);
            }
        }
    }

    // On remplit la grille à l'aide de l'interpolateur. And enjoy.
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
