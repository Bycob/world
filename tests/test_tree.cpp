#include <iostream>
#include <stdexcept>

#include <worldcore.h>
#include <worldtree.h>

using namespace world;

void testCircularSkeletton(int argc, char** argv);
void testTree(int argc, char **argv);

int main(int argc, char** argv) {
	testTree(argc, argv);
}

void testCircularSkeletton(int argc, char** argv) {
	std::cout << "Test des WeightedSkeletton avec des références circulaires" << std::endl;
	auto *skeletton = new WeightedSkeletton<TreeInfo>();

	Node<TreeInfo> * primaryNode = skeletton->getPrimaryNode();
	primaryNode->setWeight(1);
	Node<TreeInfo> * secondNode = primaryNode->createNeighbour(2);
	Node<TreeInfo> * thirdNode = secondNode->createNeighbour(3);

	//Circulaire
	thirdNode->addNeighbour(primaryNode);

	delete skeletton;
}

using namespace tree;

void testTree(int argc, char ** argv) {

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
	// TODO resoudre le leak sur le generator
	auto &skelettonGenerator = tree.addWorker<TreeSkelettonGenerator>();

	skelettonGenerator.setInclination(SideBranchPhiParameter(wrapper_d(gaussian(0.9, 0.2))));
	skelettonGenerator.setRotationOffset(SideBranchOffsetThetaParameter(wrapper_d(gaussian(0, 0.5))));
	skelettonGenerator.setForkingCount(tree::uniform_i(4, 6));
	skelettonGenerator.setSizeFactor(SideBranchSizeParameter(wrapper_d(gaussian(4.0 / 5.0, 0.1))));
	skelettonGenerator.setMaxForkingLevel(MaxLevelByWeightParameter(0.02));
	skelettonGenerator.setWeight(SideBranchWeightParameter(DefaultWeightParameter()));

	tree.addWorker<TrunkGenerator>();

	std::cout << "Generation" << std::endl;
	Collector collector;
	tree.collectWholeObject(collector);

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
