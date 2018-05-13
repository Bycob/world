#include "SimpleTreeDecorator.h"

#include <vector>

#include "math/MathsHelper.h"
#include "TrunkGenerator.h"
#include "TreeSkelettonGenerator.h"
#include "LeavesGenerator.h"

namespace world {

SimpleTreeDecorator::SimpleTreeDecorator(int maxTreesPerChunk)
        : _rng(time(NULL)), _maxTreesPerChunk(maxTreesPerChunk) {

    auto &skeletton = _model.addWorker<TreeSkelettonGenerator>();
    skeletton.setMaxForkingLevel(TreeParamsi::constant(2));
    skeletton.setForkingCount(TreeParamsi::constant(3));
    skeletton.setInclination(TreeParamsd::gaussian(0.25 * M_PI, 0.05 * M_PI));
    skeletton.setRotationOffset(TreeParamsd::gaussian(0, 0.05 * M_PI));
    skeletton.setSizeFactor(TreeParamsd::uniform_real(0.5, 0.75));

    _model.addWorker<TrunkGenerator>(12);
    _model.addWorker<LeavesGenerator>(0.2, 0.9);
}

void SimpleTreeDecorator::setModel(const Tree &model) {
    _model.setup(model);
}

void SimpleTreeDecorator::decorate(FlatWorld &world, const WorldZone &zone) {
    const double treeResolution = 5;
    if (zone->getMaxResolution() < treeResolution ||
        treeResolution <= zone->getMinResolution())
        return;

    vec3d chunkSize = zone->getDimensions();
    vec3d offset = zone->getAbsoluteOffset();

    std::vector<vec2d> positions;
    std::uniform_real_distribution<double> distribX(0, chunkSize.x);
    std::uniform_real_distribution<double> distribY(0, chunkSize.y);

    IGround &ground = world.ground();

    for (int i = 0; i < _maxTreesPerChunk; i++) {
        // On génère une position pour l'arbre
        vec2d position(distribX(_rng), distribY(_rng));

        // On vérifie que les autres arbres ne sont pas trop près
        bool addTree = true;

        for (const vec2d &prevPos : positions) {

            if (prevPos.squaredLength(position) < 6) {
                addTree = false;
                break;
            }
        }

        if (!addTree)
            continue;

        // Détermination de l'altitude de l'arbre
        double altitude =
            ground.observeAltitudeAt(zone, position.x, position.y);
        vec3d pos3D(position.x, position.y, altitude - offset.z);

        // We don't generate the tree if the ground level is not in the chunk at
        // this location
        if (pos3D.z < 0 || pos3D.z >= chunkSize.z) {
            continue;
        }
        // std::cout << pos3D << std::endl;

        // Création de l'arbre
        Tree &tree = world.addObject<Tree>(zone);
        tree.setup(_model);
        tree.setPosition3D(pos3D);

        // Remember position
        positions.push_back(position);
    }
}
} // namespace world
