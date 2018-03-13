#ifndef WORLD_FIRSTPERSONWORLDEXPLORER_H
#define WORLD_FIRSTPERSONWORLDEXPLORER_H

#include <worldapi/worldapidef.h>

#include "World.h"
#include "WorldCollector.h"

class WORLDAPI_EXPORT FirstPersonWorldExplorer {
public:
    FirstPersonWorldExplorer(double minAngle = 0.02, double punctumProximum = 1);

    void setOrigin(const maths::vec3d &origin);
    void setMinAngle(double minAngle);
    void setPunctumProximum(double punctumProximum);

    template <typename T>
    void explore(T & world, IWorldCollector<T> & collector);

    double getDetailSizeAt(const maths::vec3d &pos);
private:
    double _angularResolution;
    double _punctumProximum;
    maths::vec3d _origin;
};

#include "FirstPersonWorldExplorer.inl"

#endif //WORLD_FIRSTPERSONWORLDEXPLORER_H
