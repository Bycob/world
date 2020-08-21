#ifndef WORLD_TREEPLACEHOLDERWORKER_H
#define WORLD_TREEPLACEHOLDERWORKER_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "world/assets/Color.h"

namespace world {

class WORLDAPI_EXPORT TreePlaceholderWorker : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TreePlaceholderWorker(Color4d trunkColor = {}, Color4d leavesColor = {},
                          double height = 0);

    void processInstance(TreeInstance &tree, double resolution);

    TreePlaceholderWorker *clone() const override;

private:
    std::mt19937 _rng;

    // in meters
    double _height;
    double _width;

    // in pixels
    u32 _texHeight;
    u32 _texWidth;

    // in meters
    double _trunkWidth;
    double _trunkHeight;

    Color4d _trunkColor;
    Color4d _leavesColor;
};
} // namespace world


#endif // WORLD_TREEPLACEHOLDERWORKER_H
