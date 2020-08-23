#ifndef WORLD_TREESKELETTONWORKER_H
#define WORLD_TREESKELETTONWORKER_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

/** Base class to all skeletton workers. */
class WORLDAPI_EXPORT TreeSkelettonWorker : public ITreeWorker {
public:
    virtual void randomize(){};

protected:
    // Utility functions may be added in the future.
};
} // namespace world

#endif // WORLD_TREESKELETTONWORKER_H
