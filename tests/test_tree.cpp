#include <iostream>
#include <stdexcept>

#include <world/core.h>
#include <world/tree.h>

using namespace world;

void testCircularSkeletton(int argc, char **argv);
void testTreeGroup(int argc, char **argv);
void testTree(int argc, char **argv);
void testGrass();
void testRock();

int main(int argc, char **argv) {
    // testTree(argc, argv);
    testRock();
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

void testTreeGroup(int argc, char **argv) {
    Collector collector;
    TreeGroup treeGroup;
    treeGroup.addTree(vec3d{1, 1, 1});
    treeGroup.addTree(vec3d{2, 5, 1});

    treeGroup.collectAll(collector, 1);
}

void testTree(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        std::string key = arg.substr(0, 1);
        std::string value = arg.substr(1);

        // to do
    }

    std::cout << "Création du dossier \"tree\"..." << std::endl;
    createDirectories("assets/tree");

    std::cout << "Parametrage de l'arbre" << std::endl;
    Tree tree;
    auto &skelGen = tree.addWorker<TreeSkelettonGenerator>();

//#define SIDE_BRANCH
#ifdef SIDE_BRANCH
    skelGen.setInclination(SideBranchd::Phi(TreeParamsd::gaussian(0.9, 0.2)));
    skelGen.setTheta(SideBranchd::Theta(TreeParamsd::gaussian(0, 0.5)));
    skelGen.setForkingCount(
        TreeParamsi::WeightThreshold(0.02, TreeParamsi::uniform_int(3, 5)));
    skelGen.setSize(SideBranchd::Size(TreeParamsd::uniform_real(0.05, 0.5)));
    skelGen.setWeight(SideBranchd::Weight(TreeParamsd::gaussian(0.5, 0.1)));
#else  // SIDE_BRANCH
    skelGen.setInclination(
        TreeParamsd::PhiOffset(TreeParamsd::gaussian(0.5, 0.1)));
    skelGen.setTheta(TreeParamsd::UniformTheta(TreeParamsd::gaussian(0, 0.3)));
    skelGen.setForkingCount(
        TreeParamsi::WeightThreshold(0.01, TreeParamsi::uniform_int(3, 6)));
    skelGen.setSize(TreeParamsd::SizeByWeight(1));
    skelGen.setWeight(TreeParamsd::DefaultWeight());
#endif // SIDE_BRANCH

    tree.addWorker<TrunkGenerator>();
    tree.addWorker<LeavesGenerator>();

    std::cout << "Generation" << std::endl;
    Collector collector;
    tree.collectAll(collector, 15);

    std::cout << "Converting skeletton into 3D model..." << std::endl;
    std::shared_ptr<Mesh> mesh(tree.getSkeletton().convertToMesh());

    std::cout << "Ecriture du modele du squelette..." << std::endl;
    ObjLoader file;
    Scene scene;
    scene.addObject(Object3D(*mesh));
    file.write(scene, "assets/tree/skeletton");

    std::cout << "Ecriture du modele de l'arbre..." << std::endl;
    Scene scene2;
    collector.fillScene(scene2);
    file.write(scene2, "assets/tree/tree");
}


void testGrass() {
    Collector collector;
    collector.addStorageChannel<Object3D>();
    collector.addStorageChannel<Material>();
    collector.addStorageChannel<Image>();

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
        VoxelOps::ball(voxels, {2, 2, 2}, 3.5, 2);
        VoxelOps::ball(voxels, {-2.5, -2, -2.5}, 5, 1);
    };

    ops();

    Mesh mesh;
    voxels.fillMesh(mesh);

    Scene scene;
    scene.addObject(Object3D(mesh));

    createDirectories("assets/rocks/");
    ObjLoader().write(scene, "assets/rocks/rocks");
    std::cout << "Wrote files to assets/rocks" << std::endl;
}