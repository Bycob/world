#include <iostream>
#include <stdexcept>

#include <worldapi/ObjLoader.h>
#include <worldapi/Scene.h>
#include <worldapi/Mesh.h>
#include <worldapi/IOUtil.h>
#include <worldapi/tree/TreeSkelettonGenerator.h>
#include <worldapi/tree/TreeGenerator.h>
#include <worldapi/tree/TreeSkeletton.h>
#include <worldapi/tree/Tree.h>

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
	TreeSkelettonGenerator generator;

	generator.setInclination(SideBranchPhiParameter(wrapper_d(gaussian(0.9, 0.2))));
	generator.setRotationOffset(SideBranchOffsetThetaParameter(wrapper_d(gaussian(0, 0.5))));
	generator.setForkingCount(tree::uniform_i(4, 6));
	generator.setSizeFactor(SideBranchSizeParameter(wrapper_d(gaussian(4.0 / 5.0, 0.1))));
	generator.setMaxForkingLevel(MaxLevelByWeightParameter(0.02));
	generator.setWeight(SideBranchWeightParameter(DefaultWeightParameter()));

	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		std::string key = arg.substr(0, 1);
		std::string value = arg.substr(1);

		
	}
	
	std::cout << "Création du dossier \"tree\"..." << std::endl;
	ioutil::createDirectory("trees");

	std::cout << "Génération d'un squelette d'arbre" << std::endl;
	std::unique_ptr<TreeSkeletton> treeSkeletton(generator.generate());

	std::cout << "Généré ! Conversion en modèle..." << std::endl;
	std::shared_ptr<Mesh> mesh(treeSkeletton->convertToMesh());

	std::cout << "Modèle converti ! Ecriture du modèle..." << std::endl;
	ObjLoader file;
	Scene scene;
	scene.createObject(mesh);
	file.write(scene, "trees/skeletton");

	std::cout << "Génération du tronc de l'arbre" << std::endl;
	TreeGenerator generator2;
	std::unique_ptr<Tree> tree(generator2.generate(*treeSkeletton));

	std::cout << "Généré ! Ecriture du modèle" << std::endl;
	const Mesh & trunkMesh = tree->getTrunkMesh();
	Scene scene2;
	scene2.createObject(trunkMesh);
	file.write(scene2, "trees/trunk");
}
