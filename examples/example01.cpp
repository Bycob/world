/* In this example, you can see how to create nice scenes
 * with the API. */

#include <memory>

#include <worldapi/maths/Vector.h>
#include <worldapi/maths/BoundingBox.h>
#include <worldapi/world/FirstPersonExplorer.h>
#include <worldapi/world/Collector.h>
#include <worldapi/world/WorldObject.h>
#include <worldapi/world/Chunk.h>
#include <worldapi/world/World.h>
#include <worldapi/tree/Tree.h>

using namespace world;

int main(char argc, char** argv) {
    Collector collector;


    // ==== First scene : one object
    Tree tree;

    // We collect the full tree with a choosen level of detail
    double detailSize = 0.1; // 10 cm
    tree.collectWholeObject(detailSize, collector);

    // Then we focus on a portion of this tree.
    // The tree is able to automatically change the objects already collected
    // to create a smooth 3D scene.
    BoundingBox bbox({-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});
    tree.collectPart(bbox, detailSize, collector);

    // Here we are ! Now we can gather the scene from the collector,
    // write it to a file, and then reset the collector for the next
    // scene.
    std::unique_ptr<Scene> scene1(collector.createScene());
    Obj::write(*scene1, "scene1.obj");

    collector.reset();


    // ==== Second scene : world
    World world;

    // We add a decorator to the world
    world.addWorldDecorator(new BubbleDecorator());

    // We add some objects to make it less empty
    world.addObject({0, 0, 0}, new Tree());
    world.addObject({5, 7.5, 0}, new Tree());

    // Time to collect ! Collecting a world can be done with multiple method
    // 1. directly with the world
    bbox.reset({-5, -5, -5}, {10, 10, 10});
    world.collect(bbox, detailSize, collector);

    // Save the scene
    std::unique_ptr<Scene> scene2_1(collector.createScene());
    Obj::write(*scene2_1, "scene2.1.obj");

    collector.reset();

    // 2. using an explorer
    FirstPersonExplorer explorer(0.001, 0.2);
    explorer.setOrigin({2.5, 5, 0});

    explorer.explore(world, collector);

    // Save the scene
    std::unique_ptr<Scene> scene2_2(collector.createScene());
    Obj::write(*scene2_2, "scene2.2.obj");

    collector.reset();

    // 3. explore the world directly
    double bigDetailSize = 1;
    ChunkPtr chunkPtr = world.exploreLocation({0, 0, 0}, bigDetailSize);
    world.collect(chunkPtr, collector);

    for (ChunkPtr neighbour : world.exploreNeighbourhood(chunk)) {
        world.collect(neighbour, collector);
    }

    // Save the scene
    std::unique_ptr<Scene> scene2_3(collector.createScene());
    Obj::write(*scene2_3, "scene2.3.obj");

    collector.reset();
}