#ifndef WORLD_WORLDNODE_P_H
#define WORLD_WORLDNODE_P_H

#include <map>
#include <memory>

namespace world{

        class WorldNodePrivate {
            public:

            std::map<NodeKey, std::unique_ptr<WorldNode>> _children;
            int _counter = 0;
        };

}
#endif //WORLD_WORLDNODE_P_H
