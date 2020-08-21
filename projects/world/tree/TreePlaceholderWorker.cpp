#include "TreePlaceholderWorker.h"

#include "Tree.h"
#include "world/assets/MeshOps.h"
#include "world/assets/ImageUtils.h"

namespace world {
WORLD_REGISTER_CHILD_CLASS(ITreeWorker, TreePlaceholderWorker,
                           "TreePlaceholderWorker");

TreePlaceholderWorker::TreePlaceholderWorker(Color4d trunkColor,
                                             Color4d leavesColor, double height)
        : _rng(std::random_device()()), _height(height), _width(_height * 0.35),
          _texHeight(256), _texWidth(128), _trunkWidth(_width * 0.2),
          _trunkHeight(_height * 0.3), _trunkColor(trunkColor),
          _leavesColor(leavesColor) {}

void TreePlaceholderWorker::processInstance(TreeInstance &tree,
                                            double resolution) {
    if (tree._tree.isTwoMeshes(resolution)) {
        return;
    }

    auto &lod = tree.getLodByResolution(resolution);
    Mesh &mesh = lod.getMesh(0);

    if (!mesh.empty()) {
        return;
    }

    // Create mesh
    double hwidth = _width / 2;
    mesh.newVertex({0, -hwidth, 0}, vec3d::X(), {0, 1});
    mesh.newVertex({0, hwidth, 0}, vec3d::X(), {1, 1});
    mesh.newVertex({0, hwidth, _height}, vec3d::X(), {1, 0});
    mesh.newVertex({0, -hwidth, _height}, vec3d::X(), {0, 0});
    mesh.newVertex({-hwidth, 0, 0}, vec3d::Y(), {0, 1});
    mesh.newVertex({hwidth, 0, 0}, vec3d::Y(), {1, 1});
    mesh.newVertex({hwidth, 0, _height}, vec3d::Y(), {1, 0});
    mesh.newVertex({-hwidth, 0, _height}, vec3d::Y(), {0, 0});

    mesh.newFace(0, 1, 2);
    mesh.newFace(0, 2, 3);
    mesh.newFace(4, 5, 6);
    mesh.newFace(4, 6, 7);

    MeshOps::singleToDoubleSided(mesh);

    // Create texture
    auto &texture = lod.getTexture(0);
    texture = Image(_texWidth, _texHeight, ImageType::RGBA);
    ImageUtils::fill(texture, {0, 0, 0, 0});

    // trunk
    vec2d trunkDims{_trunkWidth / _width * _texWidth,
                    _trunkHeight / _height * _texHeight};
    double trunkX = _texWidth / 2.;
    ImageUtils::drawPolygon(texture,
                            {{trunkX - trunkDims.x / 2, 0},
                             {trunkX + trunkDims.x / 2, 0},
                             {trunkX + trunkDims.x / 3, trunkDims.y},
                             {trunkX - trunkDims.x / 3, trunkDims.y}},
                            _texWidth * 0.2, _trunkColor);

    // leaves
    std::uniform_real_distribution<double> xdistrib(0.2 * _texWidth,
                                                    0.8 * _texWidth);
    std::uniform_real_distribution<double> ydistrib(
        _trunkHeight / _height * 0.5 * _texHeight, 0.9 * _texHeight);

    std::vector<vec2d> leavesPoints;
    for (u32 i = 0; i < 10; ++i) {
        leavesPoints.emplace_back(xdistrib(_rng), ydistrib(_rng));
    }

    ImageUtils::drawPolygon(texture, leavesPoints, _texWidth * 0.05,
                            _leavesColor);
}

TreePlaceholderWorker *TreePlaceholderWorker::clone() const {
    return new TreePlaceholderWorker(*this);
}
} // namespace world