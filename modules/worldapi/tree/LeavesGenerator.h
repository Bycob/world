
#ifndef WORLD_LEAVES_GENERATOR_H
#define WORLD_LEAVES_GENERATOR_H

#include "core/WorldConfig.h"

#include "ITreeWorker.h"

namespace world {

class WORLDAPI_EXPORT LeavesGenerator : public ITreeWorker {
public:
    LeavesGenerator();

    void process(Tree &tree) override;

private:
};

} // namespace world

#endif // WORLD_LEAVES_GENERATOR_H
