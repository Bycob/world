#ifndef WORLD_IINSTANCEGENERATOR_H
#define WORLD_IINSTANCEGENERATOR_H

#include "world/core/WorldConfig.h"

#include <list>

#include "InstanceDistribution.h"
#include "ICollector.h"

namespace world {

class Template {
public:
    /** One object at one resolution */
    struct Item {
        std::vector<SceneNode> _nodes;
        double _minRes;

        Item &add(const SceneNode &node) {
            _nodes.push_back(node);
            return *this;
        }
    };

    Template() = default;

    Template(const SceneNode &node);

    void insert(const Item &item);

    void insert(double resolution, const SceneNode &node);

    void insert(const SceneNode &node);

    // Return ptr is bad design :(
    /** Returns Item available at the given resolution, or nullptr if no item
     * is available at this resolution. */
    Item *getAt(double resolution);

private:
    /// Scene nodes that will be collected, at different resolution
    std::list<Item> _items;
};

// TODO Force every instance generator to inherit this class
// (by specifying TGenerator in the InstancePool)
class WORLDAPI_EXPORT IInstanceGenerator {
public:
    std::vector<SceneNode> collectTemplates(ICollector &collector,
                                            const ExplorationContext &ctx);

    HabitatFeatures randomize();

private:
};

} // namespace world

#endif // WORLD_IINSTANCEGENERATOR_H
