
#ifndef WORLD_QUICKLEAVES_H
#define WORLD_QUICKLEAVES_H

#include <world/core.h>
#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

class WORLDAPI_EXPORT QuickLeaves : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    QuickLeaves(double maxResolution = 5, const Color4d &color = {0, 1, 0});

    void setColor(const Color4d &color);

    void processInstance(TreeInstance &tree, double resolution) override;

    void processTree(Tree &tree, double resolution) override;

    QuickLeaves *clone() const override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    std::mt19937 _rng;

    double _maxResolution;
    Color4d _color;


    void addNode(const TreeNode *node, BoundingBox &bbox, int depth = 0);

    void generateTexture(Image &img);
};
} // namespace world

#endif // WORLD_QUICKLEAVES_H
