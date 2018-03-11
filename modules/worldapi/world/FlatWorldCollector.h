#ifndef WORLD_FLATWORLDCOLLECTOR_H
#define WORLD_FLATWORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include "WorldCollector.h"

class FlatWorldCollector : public WorldCollector{
public:
    FlatWorldCollector();
    ~FlatWorldCollector() override;

};


#endif //WORLD_FLATWORLDCOLLECTOR_H
