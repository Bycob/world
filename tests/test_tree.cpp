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
	auto &skelGen = tree.addWorker<TreeSkelettonGenerator>();

	/*skelGen.setInclination(SideBranch::phi(Params::gaussian(0.9, 0.2)));
	skelGen.setRotationOffset(SideBranch::offsetTheta(gaussian(0, 0.5)));
	skelGen.setForkingCount(uniform_i(4, 6));
	skelGen.setSizeFactor(SideBranch::size(gaussian(4.0 / 5.0, 0.1)));
	skelGen.setMaxForkingLevel(MaxLevelByWeight(0.02));
	skelGen.setWeight(SideBranch::weight(Trees::DefaultWeight()));*/
	skelGen.setInclination(TreeParamsd::gaussian(0.9, 0.1));
	skelGen.setRotationOffset(TreeParamsd::gaussian(0, 0.5));
	skelGen.setForkingCount(TreeParamsi::uniform_int(2, 6));
	skelGen.setSizeFactor(TreeParamsd::gaussian(2.5 / 5.0, 0.05));
	skelGen.setMaxForkingLevel(TreeParamsi::MaxLevelByWeight(0.02));
	skelGen.setWeight(TreeParamsd::DefaultWeight());

	tree.addWorker<TrunkGenerator>();

	std::cout << "Generation" << std::endl;
	Collector collector;
	tree.collectAll(collector, 5);

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
