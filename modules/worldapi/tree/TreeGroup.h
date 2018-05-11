#ifndef WORLD_FOREST_H
#define WORLD_FOREST_H

#include "core/WorldConfig.h"

namespace world {

/** A TreeGroup enables several trees to be rendered as
 * a single mesh. This is useful when the trees are really
 * far (ie a very low LOD) and you want to limit the number
 * of different objects in the scene. */
class WORLDAPI_EXPORT TreeGroup {
public:
    TreeGroup();

private:
};

} // namespace world

#endif // WORLD_FOREST_H
