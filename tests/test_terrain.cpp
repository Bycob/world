#include <iostream>
#include <stdexcept>

#include <armadillo/armadillo>

#include <worldapi/objfile.h>
#include <worldapi/Material.h>
#include <worldapi/interop.h>
#include <worldapi/ioutil.h>
#include <worldapi/maths/perlin.h>
#include <worldapi/terrain/terrain.h>
#include <worldapi/terrain/TerrainGenerator.h>
#include <worldapi/terrain/TerrainTexmapBuilder.h>

using namespace perlin;

//DECLARATIONS
void testTinyObjLoader(int argc, char** argv);
void testPerlin(int argc, char** argv);


//MAIN
int main(int argc, char** argv) {
	testPerlin(argc, argv);
}



//IMPLEMENTATIONS
void testTinyObjLoader(int argc, char** argv) {
	if (argc >= 2) {
		try {
			ObjLoader obj(argv[1]);
			std::cout << "Lecture du mesh" << std::endl;
			obj.printDebug();

			std::vector<std::shared_ptr<Mesh>> output;
			obj.getMeshes(output);
			std::cout << "Mesh lu !" << std::endl;
			std::shared_ptr<Mesh> mesh2 = output.at(0);

			std::cout << "Reecriture du fichier sous un autre nom" << std::endl;
			ObjLoader obj2;
			obj2.addMesh(mesh2);

			obj2.write("tests/result.obj");
		}
		catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
	else {
		std::cout << "Usage : test [nom d'un fichier]" << std::endl;
	}
}

void testPerlin(int argc, char** argv) {

	//ANALYSE DES ARGUMENTS

	int size = 257;
	int octaves = 4;
	int freq = 4;
	float persistence = 0.3;

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
		catch (std::exception & e) {
			std::cout << "erreur dans la ligne de commande, utilisation des param�tres par d�faut..." << std::endl;
		}
	}

	//CREATION DU DOSSIER DE TESTS
	std::cout << "Cr�ation du dossier de tests..." << std::endl;
	ioutil::createDirectory("tests");

	// CREATION D'UN PERLIN REPETABLE
	std::cout << "Perlin r�p�table" << std::endl;
	auto repeatable = generatePerlinNoise2D(size, 0, 1, freq, persistence, true);
	img::Image repeat(repeatable);
	repeat.write("tests/repeat.png");
    
	//CREATION DU GENERATEUR

	PerlinTerrainGenerator generator(size, 0, octaves, freq, persistence);

	//GENERATION DE L'IMAGE DU TERRAIN

	std::cout << "G�n�ration du terrain..." << std::endl;
	std::unique_ptr<Terrain> terrain = generator.generate();

	std::cout << "ecriture de l'image du terrain..." <<std::endl;
	img::Image image = terrain->convertToImage();
	try {
		image.write("tests/terrain.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	//ECRITURE DU FICHIER OBJ DU TERRAIN

	std::cout << "ecriture du mesh du terrain" << std::endl;
	ObjLoader file;
	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(terrain->convertToMesh());
	file.addMesh(mesh);
	
	std::shared_ptr<Material> material = std::make_shared<Material>("material01");
	material->setMapKd("test2_tex.png");
	material->setKs(0, 0, 0);
	file.addMaterial(material);

	try {
		file.write("tests/terrain.obj");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	//GENERATION DE LA TEXTURE DU TERRAIN

	std::cout << "creation de la texture" << std::endl;

	//---
	TerrainTexmapBuilder texmapBuilder(0, 255);

	std::vector<ColorPart> slice1;
	slice1.push_back(ColorPart(47.0 / 255, 128.0 / 255, 43.0 / 255, 0.75));
	slice1.push_back(ColorPart(65.0 / 255, 53.0 / 255, 22.0 / 255, 0.25));

	std::vector<ColorPart> slice2;
	slice2.push_back(ColorPart(47.0 / 255, 128.0 / 255, 43.0 / 255, 0.15));
	slice2.push_back(ColorPart(65.0 / 255, 53.0 / 255, 22.0 / 255, 0.25));
	slice2.push_back(ColorPart(0.25, 0.25, 0.25, 0.6));

	std::vector<ColorPart> slice3;
	slice3.push_back(ColorPart(1, 1, 1, 0.7));
	slice3.push_back(ColorPart(0.25, 0.25, 0.25, 0.3));
	
	texmapBuilder.addSlice(1, slice1);
	texmapBuilder.addSlice(180, slice2);
	texmapBuilder.addSlice(230, slice3);
	//---

	arma::Mat<double> randomArray = generatePerlinNoise2D(size * 8, 0, 7, 16, 0.9);
	img::Image texture = generator.generateTexture(*terrain, texmapBuilder, randomArray);

	std::cout << "ecriture de la texture..." << std::endl;
	try {
		texture.write("tests/test2_tex.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	std::cout << "ecriture de la carte" << std::endl;
	try {
		img::Image(texmapBuilder.convertToMap()).write("tests/test2_map.png");
	}
	catch (std::exception & e) {
		std::cout << "erreur : " << e.what() << std::endl;
	}

	// CREATION DES SUBDIVISIONS DU TERRAIN
	std::cout << "g�n�ration des subdivisions" << std::endl;
	generator.generateSubdivisions(*terrain, 4, 1);
	std::cout << "g�n�ration termin�e !" << std::endl;

	std::cout << "Ecriture du fichier obj d'un sous-terrain..." << std::endl;
	ObjLoader subterrainFile;
	Terrain & subterrain = terrain->getSubterrain(0, 0);
	std::shared_ptr<Mesh> submesh = std::shared_ptr<Mesh>(subterrain.convertToSubmesh());
	subterrainFile.addMesh(submesh);
	subterrainFile.write("tests/subterrain");
	std::cout << "Fichier �crit !" << std::endl;
	
	std::cout << "Ecriture de l'image associ�e..." << std::endl;
	img::Image image2 = subterrain.convertToImage();
	image2.write("tests/subterrain.png");
	std::cout << "Image �crite !" << std::endl;
}