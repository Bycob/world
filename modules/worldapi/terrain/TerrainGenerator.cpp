
#include "TerrainGenerator.h"
#include "../maths/Perlin.h"
#include "../Image.h"

using namespace img;
using namespace perlin;
using namespace arma;

TerrainGenerator::TerrainGenerator(int size) : _size(size) {

}

TerrainGenerator::~TerrainGenerator() {

}

void TerrainGenerator::setSize(int size) {
	_size = size;
}

TerrainSubdivisionTree * TerrainGenerator::generateSubdivisions(Terrain & terrain, int subdivideFactor, int subdivisionsCount) {
	TerrainSubdivisionTree * result = new TerrainSubdivisionTree(terrain);
	generateSubdivisionLevels(*result, subdivideFactor, subdivisionsCount);
	return result;
}

void TerrainGenerator::generateSubdivisionLevels(TerrainSubdivisionTree & tree, int subdivideFactor, int subdivisionsCount) {
	generateSubdivisionLevel(tree, subdivideFactor);

	// Subdivisions d'un niveau supplémentaire (récursivité)
	if (subdivisionsCount > 1) {
		for (int x = 0; x < subdivideFactor; x++) {
			for (int y = 0; y < subdivideFactor; y++) {
				TerrainSubdivisionTree & subtree = tree.getSubtree(x, y);
				generateSubdivisionLevels(subtree, subdivideFactor, subdivisionsCount - 1);
			}
		}
	}
}

void TerrainGenerator::generateSubdivisionLevel(TerrainSubdivisionTree & tree, int subdivideFactor) {
	tree.subdivide(subdivideFactor);

	// Géneration des subdivisions à l'étage actuel
	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			generateSubdivision(tree, x, y);
		}
	}
}

//TODO check des tailles des tableaux passés en paramètres (map et randomArray)

Image TerrainGenerator::generateTexture(const Terrain & terrain, const TerrainTexmapBuilder & builder, const Mat<double> & randomArray) const {
	//TODO support de l'altitude
	auto map = builder.convertToMap();

	return generateTexture(terrain, map, randomArray);
}

//Cette méthode là ne supportera pas l'altitude.
Image TerrainGenerator::generateTexture(const Terrain & terrain, const arma::Cube<double> & map, const Mat<double> & randomArray) const {
	float scaleX = (float)terrain._array.n_rows / randomArray.n_rows;
	float scaleY = (float)terrain._array.n_cols / randomArray.n_cols;
	arma::Cube<double> result(randomArray.n_rows, randomArray.n_cols, 3);

	for (int x = 0; x < randomArray.n_rows; x++) {
		for (int y = 0; y < randomArray.n_cols; y++) {
			double x1 = (double) x / randomArray.n_rows;
			double y1 = (double) y / randomArray.n_cols;

			double z = terrain.getZInterpolated(x1, y1);
			double randParam = randomArray(x, y);

			result(x, y, 0) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 0);
			result(x, y, 1) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 1);
			result(x, y, 2) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 2);
		}
	}

	return Image(result);
}



// GENERATEUR DE PERLIN

PerlinTerrainGenerator::PerlinTerrainGenerator(int size, int offset, int octaveCount, float frequency, float persistence) :
	TerrainGenerator(size) ,
    _buffer(std::map<std::pair<int, int>, arma::Mat<double>>()),
    _offset(offset), _octaveCount(octaveCount), _frequency(frequency), _persistence(persistence) {

}

PerlinTerrainGenerator::~PerlinTerrainGenerator() {
	
}

Terrain * PerlinTerrainGenerator::generate() {
	Terrain * result = new Terrain(_size);
	_perlin.generatePerlinNoise2D(result->_array, _offset, _octaveCount, _frequency, _persistence);

	return result;
}

void PerlinTerrainGenerator::join(Terrain &terrain1, Terrain &terrain2, bool axisX, bool joinableSides) {
	_perlin.join(terrain1._array, terrain2._array,
				 axisX ? Direction::AXIS_X : Direction::AXIS_Y,
				 _octaveCount, _frequency, _persistence,
				 joinableSides);
}

void PerlinTerrainGenerator::generateSubdivisionLevel(TerrainSubdivisionTree & terrain, int subdivideFactor) {
	terrain.subdivide(subdivideFactor);

	// Pas de prébuffering

	// Géneration des buffers pour le niveau de détail actuel
	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			generateSubdivision(terrain, x, y);
		}
	}

	// Conversion des buffers en terrain
	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			adaptFromBuffer(terrain, x, y);
		}
	}

	_buffer.clear();
}

void PerlinTerrainGenerator::generateSubdivision(TerrainSubdivisionTree & terrain, int xsub, int ysub) {
	//TODO Faire plusieurs tests pour voir si on obtient un meilleur résultat en changeant les paramètres de l'algo de perlin.
	Mat<double> & mat = createInBuf(xsub, ysub, terrain.getSubtree(xsub, ysub)._terrain->_array);
	_perlin.generatePerlinNoise2D(mat, _offset, _octaveCount, _frequency, _persistence);

	double oneTerrainLength = 1.0 / terrain._subdivideFactor;

    // Harmonisation horizontale
	// -> Le joint en y doit toujours se faire avant le joint en x pour n'importe quel terrain
	if (ysub != 0) {
		_perlin.join(getBuf(xsub, ysub - 1),
			          mat,
			          Direction::AXIS_Y,
			          _octaveCount, _frequency, _persistence);
		
		if (xsub != 0) {
			_perlin.join(getBuf(xsub - 1, ysub - 1),
						  getBuf(xsub, ysub - 1),
						  Direction::AXIS_X,
						  _octaveCount, _frequency, _persistence, true);
		}
	}
    if (xsub == terrain._subdivideFactor - 1) {
        _perlin.join(getBuf(xsub - 1, ysub),
                      mat,
                      Direction::AXIS_X,
                      _octaveCount, _frequency, _persistence, true);
    }
}

void PerlinTerrainGenerator::adaptFromBuffer(TerrainSubdivisionTree & tree, int xsub, int ysub) const {
	TerrainSubdivisionTree & subtree = tree.getSubtree(xsub, ysub);
	Terrain & subterrain = *subtree._terrain;
	Mat<double> & mat = getBuf(xsub, ysub);

	int rows = subterrain._array.n_rows;
	int cols = subterrain._array.n_cols;
	int sf = tree._subdivideFactor;

	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			subterrain._array(x, y) = mat(x, y) * _subdivNoiseRatio
				+ (1 - _subdivNoiseRatio) * tree.getZInterpolated(
					((double)xsub + (double)x / rows) / sf,
					((double)ysub + (double)y / cols) / sf);
		}
	}

	subtree._noisePart = _subdivNoiseRatio;
}

TerrainGenerator * PerlinTerrainGenerator::clone() const {
	return new PerlinTerrainGenerator(_size, _offset, _octaveCount, _frequency, _persistence);
}

arma::Mat<double> & PerlinTerrainGenerator::getBuf(int x, int y) const {
	return _buffer.at(std::make_pair(x, y));
}
