#pragma once

#include "world/core/WorldConfig.h"

#include <vector>
#include <map>
#include <random>
#include <memory>

#include "world/core/TileSystem.h"
#include "ITerrainWorker.h"
#include "ReliefParameters.h"

namespace world {

struct ReliefMapEntry : public IGridElement {
    Terrain _height;
    Terrain _diff;

    ReliefMapEntry(int resolution) : _height(resolution), _diff(resolution) {}
};

/** Base class for generating relief maps.
 * A relief map is compound of 2 layers : an "offset" layer
 * and a "diff" layer. The offset is the minimum/average
 * altitude at each point, and the diff quantify the variation
 * of altitude.
 * For example, mountains have a high offset and a high diff,
 * countryside has a low offset and a low diff, and a plateau
 * would have a high offset but a low diff. */
class WORLDAPI_EXPORT ReliefMapModifier : public ITerrainWorker {
public:
    ReliefMapModifier(double width = 400000, int resolution = 400);

    void setMapResolution(int mapres);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    const ReliefMapEntry &obtainMap(int x, int y);

    void setRegion(const vec2d &center, double radius, double curvature,
                   double height, double diff);

protected:
    mutable std::mt19937 _rng;
    TileSystem _tileSystem;

    GridStorage<ReliefMapEntry> _reliefMap;


    ReliefMapEntry &provideMap(int x, int y);

    virtual void generate(Terrain &height, Terrain &heightDiff) = 0;
};

class WORLDAPI_EXPORT CustomWorldRMModifier : public ReliefMapModifier {
public:
    CustomWorldRMModifier(double biomeDensity = 1, int limitBrightness = 4);

    void setBiomeDensity(float biomeDensity);

    void setLimitBrightness(int);

    void setDifferentialLaw(const AltDiffParam &law);

protected:
    void generate(Terrain &height, Terrain &heightDiff) override;

private:
    // la largeur d'un carré unité.
    static const double PIXEL_UNIT;
    /** Le nombre moyen de biomes par bloc de 100 pixels de WorldMap.*/
    double _biomeDensity;
    /** La netteté des limites entre les biomes. En pratique c'est
     *le "p" dans l'algo de l'interpolation. */
    int _limitBrightness;

    /** Probability law for altitude offset. */
    ElevationParam _offsetLaw;
    /** Loi de probabilité du différentiel d'altitude en fonction
     *de l'altitude. */
    AltDiffParam _diffLaw;
};
} // namespace world
