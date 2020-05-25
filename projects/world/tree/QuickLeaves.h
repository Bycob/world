
#ifndef WORLD_QUICKLEAVES_H
#define WORLD_QUICKLEAVES_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"

namespace world {

class WORLDAPI_EXPORT QuickLeaves : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    QuickLeaves();

    void processInstance(TreeInstance &tree, double resolution) override;

    void processTree(Tree &tree, double resolution) override;

    QuickLeaves *clone() const override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
};
} // namespace world

#endif // WORLD_QUICKLEAVES_H
