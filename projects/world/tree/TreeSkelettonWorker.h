#ifndef WORLD_TREESKELETTONWORKER_H
#define WORLD_TREESKELETTONWORKER_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

class WORLDAPI_EXPORT TreeSkelettonWorker : public ITreeWorker {
public:
    virtual void randomize(){};

protected:
};
} // namespace world

#endif // WORLD_TREESKELETTONWORKER_H
