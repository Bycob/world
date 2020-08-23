#include <iostream>
#include <stdexcept>

#include <world/core.h>
#include <world/tree.h>
#include <world/nature/Rocks.h>

using namespace world;

void testCircularSkeletton(int argc, char **argv);
void testTree(int argc, char **argv);
void testTrees();
void testGrass();
void testRock();

int main(int argc, char **argv) {
    // testTree(argc, argv);
    testTrees();
}

void testCircularSkeletton(int argc, char **argv) {
    std::cout << "Test des WeightedSkeletton avec des références circulaires"
              << std::endl;
    auto *skeletton = new WeightedSkeletton<TreeInfo>();

    SkelettonNode<TreeInfo> *primaryNode = skeletton->getPrimaryNode();
    SkelettonNode<TreeInfo> *secondNode =
        primaryNode->createNeighbour(TreeInfo());
    SkelettonNode<TreeInfo> *thirdNode =
        secondNode->createNeighbour(TreeInfo());

    // Circulaire
    thirdNode->addNeighbour(primaryNode);

    delete skeletton;
}

void testTree(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        std::string key = arg.substr(0, 1);
        std::string value = arg.substr(1);

        // to do
    }

    std::cout << "Creating folder \"tree\"..." << std::endl;
    createDirectories("assets/tree");

    std::cout << "Setting tree parameters" << std::endl;
    Tree tree;
    auto &skelGen = tree.addWorker<TreeSkelettonHoGBasedWorker>();

    tree.addWorker<TrunkGenerator>();
    tree.addWorker<LeavesGenerator>();
    tree.addTree();

    std::cout << "Generation" << std::endl;
    Collector collector(CollectorPresets::SCENE);
    tree.collectAll(collector, 20);

    std::cout << "Converting skeletton into 3D model..." << std::endl;
    std::shared_ptr<Mesh> mesh(
        tree.getTreeInstance(0)._skeletton.convertToMesh());

    std::cout << "Writing skeletton model..." << std::endl;
    ObjLoader file;
    Scene scene;
    scene.addMesh("mesh1", *mesh);
    scene.addNode(SceneNode("mesh1"));
    file.write(scene, "assets/tree/skeletton");

    std::cout << "Writing tree model..." << std::endl;
    Scene scene2 = collector.toScene();
    file.write(scene2, "assets/tree/tree");
}

void testTrees() {
    // TODO InstancePool : enlever les templates avant d'attaquer le
    // multitextures terrain
    InstancePool treePool;
    treePool.setTemplateGenerator<Tree>();

    treePool.exportSpecies("assets/tree", 10, 20);
    std::cout << "Species exported to assets/tree" << std::endl;
}


void testGrass() {
    Collector collector(CollectorPresets::SCENE);

    Grass grass;
    grass.addBush({0.5, 0.5, 0});
    grass.addBush({0, 0, 0});
    grass.addBush({0.4, 0.1, 0});

    grass.collectAll(collector, 15);

    Scene scene;
    collector.fillScene(scene);

    ObjLoader writer;
    createDirectories("assets/grass");
    writer.write(scene, "assets/grass/bush");
    std::cout << "Wrote files to assets/grass" << std::endl;
}

void testRock() {
    // A ROCK IS A TREE, AND I AM A FLOWER POT
    VoxelField voxels({50, 50, 50}, -1);
    voxels.bbox().reset({-5, -5, -5}, {5, 5, 5});

    auto torus = [&] {
        std::uniform_real_distribution<double> dist(0, 0.1);
        std::mt19937_64 rng;

        for (u32 z = 0; z < 50; ++z) {
            for (u32 y = 0; y < 50; ++y) {
                for (u32 x = 0; x < 50; ++x) {
                    double zf = z / 10. - 2.5, xf = x / 10. - 2.5,
                           yf = y / 10. - 2.5;
                    vec3d u(xf, yf, zf);
                    vec3d d = vec3d{xf + 0.001, yf + 0.001, 0}.normalize() * 2;
                    voxels.at(x, y, z) = u.length(d) - 0.4 + dist(rng);
                }
            }
        }
    };

    auto ops = [&] {
        VoxelOps::ball(voxels, {0, 0, 0}, 3.5, 1);
        VoxelOps::ball(voxels, {-10, -5, 0}, 9, -1);
        VoxelOps::ball(voxels, {0, 0, 20}, 18.5, -1);
        VoxelOps::ball(voxels, {0, 5, -15}, 13.3, -1);
        VoxelOps::ball(voxels, {8, 0, -2}, 6.5, -1);
        VoxelOps::ball(voxels, {-2, 13, -2}, 12, -1);
        VoxelOps::ball(voxels, {0.5, -6, 0}, 3.5, -1);
        VoxelOps::ball(voxels, {5, 6, 5}, 8, -1);
        VoxelOps::ball(voxels, {-6, 6, 5}, 8.5, -1);
    };

    ops();

    Mesh mesh;
    voxels.fillMesh(mesh);

    Scene scene;
    scene.addMesh("mesh1", mesh);
    scene.addNode(SceneNode("mesh1"));

    Collector collector(CollectorPresets::SCENE);

    Rocks rocks;

    for (double x = -5; x <= 5.1; ++x) {
        for (double y = -5; y <= 5.1; ++y) {
            rocks.addRock(vec3d{x * 2, y * 2, 0});
        }
    }
    rocks.collectAll(collector, 100);

    Scene rockScene;
    collector.fillScene(rockScene);

    createDirectories("assets/rocks/");
    ObjLoader().write(scene, "assets/rocks/voxels");
    ObjLoader().write(rockScene, "assets/rocks/rocks");

    std::cout << "Wrote files to assets/rocks" << std::endl;
}