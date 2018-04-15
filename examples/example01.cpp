/* In this example, you can see how to create nice scenes
 * with the API. */

#include <memory>

#include <worldcore.h>
#include <worldtree.h>

using namespace world;

int main(char argc, char** argv) {
    Collector collector;


    // ==== First scene : one object
    Tree tree;

    // We collect the full tree with a choosen level of detail
    double resolution = 10; // 10 pts/m
    tree.collectWholeObject(resolution, collector);

    // Then we focus on a portion of this tree.
    // The tree is able to automatically change the objects already collected
    // to create a smooth 3D scene.
    BoundingBox bbox({-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});
    tree.collectPart(bbox, resolution, collector);

    // Here we are ! Now we can gather the scene from the collector,
    // write it to a file, and then reset the collector for the next
    // scene.
    Scene scene1;
    collector.fillScene(scene1);
    Obj::write(scene1, "scene1.obj");

    collector.reset();


    // ==== Second scene : world
    World world;

    // We add a decorator to the world
    world.addDecorator<BubbleDecorator>();

    // We add some objects to make it less empty
    Tree &tree1 = world.addObject<Tree>();
    tree1.setPosition3D({0, 0, 0});

    Tree &tree2 = world.addObject<Tree>();
    tree2.setPosition3D({5, 7.5, 0});

    // Time to collect ! Collecting a world can be done with multiple method
    // 1. directly with the world
    bbox.reset({-5, -5, -5}, {10, 10, 10});
    world.collect(bbox, resolution, collector);

    // Save the scene
    Scene scene2_1;
    collector.fillScene(scene2_1);
    Obj::write(scene2_1, "scene2.1.obj");

    collector.reset();

    // 2. using an explorer
    FirstPersonExplorer explorer;
    explorer.setOrigin({2.5, 5, 0});

    explorer.explore(world, collector);

    // Save the scene
    Scene scene2_2;
    collector.fillScene(scene2_2);
    Obj::write(scene2_2, "scene2.2.obj");

    collector.reset();

    // 3. explore the world directly
    double badResolution = 1; // 1 pt/m
    ChunkPtr chunkPtr = world.exploreLocation({0, 0, 0}, badResolution);
    world.collect(chunkPtr, collector);

    for (ChunkPtr neighbour : world.exploreNeighbourhood(chunk)) {
        world.collect(neighbour, collector);
    }

    // Save the scene
    Scene scene2_3;
    world.fillScene(scene2_3);
    Obj::write(scene2_3, "scene2.3.obj");

    collector.reset();
}