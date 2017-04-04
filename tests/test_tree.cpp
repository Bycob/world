#include <iostream>
#include <stdexcept>

#include <worldapi/objfile.h>
#include <worldapi/mesh.h>
#include <worldapi/ioutil.h>
#include <worldapi/trees/TreeSkelettonGenerator.h>
#include <worldapi/trees/TreeGenerator.h>
#include <worldapi/trees/TreeSkeletton.h>
#include <worldapi/trees/Tree.h>

void testCircularSkeletton(int argc, char** argv);
void testTree(int argc, char **argv);

int main(int argc, char** argv) {
	testTree(argc, argv);
}

void testCircularSkeletton(int argc, char** argv) {
	std::cout << "Test des WeightedSkeletton avec des r�f�rences circulaires" << std::endl;
	WeightedSkeletton<TreeInfo> *skeletton = new WeightedSkeletton<TreeInfo>();

	Node<TreeInfo> * primaryNode = skeletton->getPrimaryNode();
	primaryNode->setWeight(1);
	Node<TreeInfo> * secondNode = primaryNode->createNeighbour(2);
	Node<TreeInfo> * thirdNode = secondNode->createNeighbour(3);

	//Circulaire
	thirdNode->addNeighbour(primaryNode);

	delete skeletton;
}

using namespace treegen;

void testTree(int argc, char ** argv) {
	TreeSkelettonGenerator generator;

	generator.setInclination(SideBranchPhiParameter(wrapper_d(gaussian(0.9, 0.2))));
	generator.setRotationOffset(SideBranchOffsetThetaParameter(wrapper_d(gaussian(0, 0.5))));
	generator.setForkingCount(treegen::uniform_i(4, 6));
	generator.setSizeFactor(SideBranchSizeParameter(wrapper_d(gaussian(4.0 / 5.0, 0.1))));
	generator.setMaxForkingLevel(MaxLevelByWeightParameter(0.02));
	generator.setWeight(SideBranchWeightParameter(DefaultWeightParameter()));

	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		std::string key = arg.substr(0, 1);
		std::string value = arg.substr(1);

		
	}
	
	std::cout << "Cr�ation du dossier \"tree\"..." << std::endl;
	ioutil::createDirectory("trees");

	std::cout << "G�n�ration d'un squelette d'arbre" << std::endl;
	std::unique_ptr<TreeSkeletton> treeSkeletton = generator.generate();

	std::cout << "G�n�r� ! Conversion en mod�le..." << std::endl;
	std::shared_ptr<Mesh> mesh(treeSkeletton->convertToMesh());

	std::cout << "Mod�le converti ! Ecriture du mod�le..." << std::endl;
	ObjLoader file;
	file.addMesh(mesh);
	file.write("trees/skeletton");

	std::cout << "G�n�ration du tronc de l'arbre" << std::endl;
	TreeGenerator generator2;
	std::unique_ptr<Tree> tree = generator2.generate(*treeSkeletton);

	std::cout << "G�n�r� ! Ecriture du mod�le" << std::endl;
	const Mesh & trunkMesh = tree->getTrunkMesh();
	ObjLoader file2;
	file2.addMesh(trunkMesh);
	file2.write("trees/trunk");
}