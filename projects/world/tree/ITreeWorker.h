#ifndef WORLD_ITREEWORKER_H
#define WORLD_ITREEWORKER_H

#include "world/core/WorldConfig.h"

#include "world/core/ICloneable.h"
#include "world/core/WorldFile.h"

namespace world {

class Tree;
class TreeInstance;

class WORLDAPI_EXPORT ITreeWorker : public ICloneable<ITreeWorker>,
                                    public ISerializable {
public:
    virtual void processInstance(TreeInstance &tree, double resolution) {}

    virtual void processTree(Tree &tree, double resolution) {}
};
} // namespace world

#endif // WORLD_ITREEWORKER_H
