#ifndef WORLD_IINSTANCEGENERATOR_H
#define WORLD_IINSTANCEGENERATOR_H

#include "world/core/WorldConfig.h"

#include <list>

#include "InstanceDistribution.h"
#include "ICollector.h"

namespace world {

class WORLDAPI_EXPORT Template {
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

    vec3d _position;
    vec3d _rotation;
    vec3d _scale = {1};
    int _id = 0;


    Template() = default;

    Template(const SceneNode &node);

    void insert(const Item &item);

    void insert(double resolution, const SceneNode &node);

    void insert(double resolution, const std::vector<SceneNode> &nodes);

    void insert(const SceneNode &node);

    // Return ptr is bad design :(
    /** Returns Item available at the given resolution, or nullptr if no item
     * is available at this resolution. */
    Item *getAt(double resolution);

    /** Get a scene node from the template, if any, else returns an empty
     * SceneNode. */
    SceneNode getDefaultNode();

private:
    /// Scene nodes that will be collected, at different resolution
    std::vector<Item> _items;
};


class WORLDAPI_EXPORT IInstanceGenerator {
public:
    virtual ~IInstanceGenerator() = default;

    virtual std::vector<Template> collectTemplates(
        ICollector &collector, const ExplorationContext &ctx,
        double maxRes) = 0;

    virtual HabitatFeatures randomize() = 0;

    /** Create a new fresh instance of this generator. */
    virtual IInstanceGenerator *newInstance() = 0;

private:
};

} // namespace world

#endif // WORLD_IINSTANCEGENERATOR_H
