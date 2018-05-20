#include <iostream>
#include <stdexcept>

#include <armadillo/armadillo>

#include <worldcore.h>
#include <worldterrain.h>

using namespace arma;
using namespace world;

//DECLARATIONS
void testTinyObjLoader(int argc, char** argv);
void testRepeatable(int argc, char** argv);
void testPerlin(int argc, char** argv);


//MAIN
int main(int argc, char** argv) {
	testPerlin(argc, argv);
}



//IMPLEMENTATIONS
void testTinyObjLoader(int argc, char** argv) {
	if (argc >= 2) {
		try {
			ObjLoader obj;
			std::cout << "Lecture du mesh" << std::endl;

			Scene scene;
			obj.read(scene, argv[1]);
			std::cout << "Mesh lu !" << std::endl;

			std::vector<Object3D*> output;
			scene.getObjects(output);

			std::cout << "Reecriture du fichier sous un autre nom" << std::endl;
			obj.write(scene, "assets/terrain/result.obj");
		}
		catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
	else {
		std::cout << "Usage : test [nom d'un fichier]" << std::endl;
	}
}

void testRepeatable(int argc, char** argv) {

    int size = 257;
    int octaves = 4;
    double freq = 4;
    double persistence = 0.4;

	Perlin perlin;

    // CREATION D'UN PERLIN REPETABLE
    std::cout << "Génération de bruit de perlin répétable..." << std::endl;
    Mat<double> repeatable = perlin.generatePerlinNoise2D(size, {octaves, persistence, true, 0, freq, 0, 0});
    Image repeat(repeatable);
    repeat.write("assets/terrain/repeat.png");

    std::cout << "Génération du terrain associé..." << std::endl;
    Terrain tr(repeatable);
    Scene repeatableobj;
    std::unique_ptr<Mesh> mesh(tr.createMesh());
	repeatableobj.addObject(Object3D(*mesh));

	ObjLoader loader;
    loader.write(repeatableobj, "assets/terrain/repeatable");
}

void testPerlin(int argc, char** argv) {

	//ANALYSE DES ARGUMENTS

	int size = 129;
	int octaves = 4;
	double freq = 4;
	double persistence = 0.3;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		try {
			int argval = std::stoi(arg.substr(1));
			if ("s" == arg.substr(0, 1)) {
				size = argval;
			}
			if ("o" == arg.substr(0, 1)) {
				octaves = argval;
			}
			if ("f" == arg.substr(0, 1)) {
				freq = argval;
			}
		}
		catch (std::exception &) {
			std::cout << "erreur dans la ligne de commande, utilisation des paramètres par défaut..." << std::endl;
		}
	}

	//CREATION DU DOSSIER DE TESTS
	std::cout << "Création du dossier de tests..." << std::endl;
	createDirectories("assets/terrain");

	//CREATION DU GENERATEUR

	PerlinTerrainGenerator generator(octaves, freq, persistence);

	//GENERATION DE L'IMAGE DU TERRAIN

	std::cout << "Génération du terrain..." << std::endl;
    Terrain terrain(size);
    generator.processTerrain(terrain);

	std::cout << "ecriture de l'image du terrain..." <<std::endl;
	Image image = terrain.createImage();
	try {
		image.write("assets/terrain/terrain.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	//ECRITURE DU FICHIER OBJ DU TERRAIN

	ObjLoader meshIO;

	std::cout << "Conversion du terrain en mesh... " << std::endl;
	Scene scene1;
	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(terrain.createMesh());
	scene1.addObject(Object3D(*mesh));
	
	std::shared_ptr<Material> material = std::make_shared<Material>("material01");
	material->setMapKd("test2_tex.png");
	material->setKs(0, 0, 0);
	scene1.addMaterial(material);

	std::cout << "Ecriture du mesh dans un fichier .obj..." << std::endl;

	try {
		meshIO.write(scene1, "assets/terrain/terrain.obj");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	//GENERATION DE LA TEXTURE DU TERRAIN

	std::cout << "creation de la texture" << std::endl;
	
	AltitudeTexturer texturer;

	auto &colorMap = texturer.getColorMap();
	colorMap.addPoint({0.15, 0.5}, Color4u(209, 207, 153)); // Sand
	colorMap.addPoint({0.31, 0}, Color4u(209, 207, 153)); // Sand
	colorMap.addPoint({0.31, 1}, Color4u(209, 207, 153)); // Sand
	colorMap.addPoint({0.35, 0}, Color4u(72, 132, 85)); // Grass blueish
	colorMap.addPoint({0.35, 1}, Color4u(144, 183, 123)); // Grass yellowish
	colorMap.addPoint({0.5, 0}, Color4u(114, 90, 48)); // Light dirt
	colorMap.addPoint({0.53, 1}, Color4u(96, 76, 40)); // Dark dirt
	colorMap.addPoint({0.65, 0}, Color4u(160, 160, 160)); // Rock
	colorMap.addPoint({0.8, 1}, Color4u(160, 160, 160)); // Rock
	colorMap.addPoint({1, 0}, Color4u(244, 252, 250)); // Snow
	colorMap.addPoint({1, 1}, Color4u(244, 252, 250)); // Snow

	terrain.setTexture(Image(512, 512, ImageType::RGB));
    texturer.processTerrain(terrain);
	const Image &texture = terrain.getTexture();

	std::cout << "ecriture de la texture..." << std::endl;
	try {
		texture.write("assets/terrain/test2_tex.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	std::cout << "ecriture de la carte" << std::endl;
	std::unique_ptr<Image> colorMapImg(colorMap.createImage());
	try {
		colorMapImg->write("assets/terrain/test2_map.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}
}

void testSubdivisions(int argc, char** argv) {
	ObjLoader meshIO;
	Terrain terrain(129);
	PerlinTerrainGenerator generator;
    generator.processTerrain(terrain);

	TerrainSubdivisionGenerator subdiv;

	// CREATION DES SUBDIVISIONS DU TERRAIN
	std::cout << "génération des subdivisions" << std::endl;
	TerrainSubdivisionTree * terrainTree = subdiv.generateSubdivisions(terrain, 4, 1);
	std::cout << "génération terminée !" << std::endl;

	std::cout << "Conversion en mesh de 4 sous-terrains..." << std::endl;
	Scene subterrainScene;
	TerrainSubdivisionTree & subtree = terrainTree->getSubtree(0, 0);
	std::shared_ptr<Mesh> submesh = std::shared_ptr<Mesh>(subtree.convertToSubmesh());
	subterrainScene.addObject(Object3D(*submesh));
	/*subterrainScene.addObject(Object3D(std::shared_ptr<Mesh>(terrainTree->getSubtree(1, 0).convertToSubmesh())));
	subterrainScene.addObject(Object3D(std::shared_ptr<Mesh>(terrainTree->getSubtree(1, 1).convertToSubmesh())));
	subterrainScene.addObject(Object3D(std::shared_ptr<Mesh>(terrainTree->getSubtree(0, 1).convertToSubmesh())));*/

	std::cout << "Ecriture du fichier .obj..." << std::endl;
	meshIO.write(subterrainScene, "assets/terrain/subterrain");
	std::cout << "Fichier écrit !" << std::endl;

	std::cout << "Ecriture de l'image associée..." << std::endl;
	Image image2 = subtree.terrain().createImage();
	image2.write("assets/terrain/subterrain.png");
	std::cout << "Image écrite !" << std::endl;
}