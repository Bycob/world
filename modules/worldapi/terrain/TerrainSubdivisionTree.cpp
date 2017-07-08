#include "TerrainSubdivisionTree.h"

#include <worldapi/maths/mathshelper.h>
#include <worldapi/maths/interpolation.h>

using namespace maths;

TerrainSubdivisionTree::TerrainSubdivisionTree(const Terrain & terrain) :
	_terrain(std::make_unique<Terrain>(terrain)) {

}

TerrainSubdivisionTree::TerrainSubdivisionTree(Terrain && terrain) :
	_terrain(std::make_unique<Terrain>(terrain)) {

}

TerrainSubdivisionTree::~TerrainSubdivisionTree() {

}

double TerrainSubdivisionTree::getZ(double x, double y, int stage) const {
	const TerrainSubdivisionTree & terrain = getSubtreeAt(x, y, stage);

	double posXd, posYd;
	getLocalPosition(terrain, x, y, posXd, posYd);

	return terrain.getZ(posXd, posYd);
}

double TerrainSubdivisionTree::getZInterpolated(double x, double y, int stage) const {
	const TerrainSubdivisionTree & terrain = getSubtreeAt(x, y, stage);

	// Positions flottantes sur le subterrain selectionné
	double posXd, posYd;
	getLocalPosition(terrain, x, y, posXd, posYd);

	return terrain.getZInterpolated(posXd, posYd);
}

bool TerrainSubdivisionTree::isSubdivided() const {
	return _subdivideFactor != 0;
}

int TerrainSubdivisionTree::getLevelDiff(const TerrainSubdivisionTree & terrain) const {
	return _subdivisionStage - terrain._subdivisionStage;
}

void TerrainSubdivisionTree::subdivide(int subdivideFactor) {
	_subdivideFactor = subdivideFactor;
	_subterrain = std::vector<std::unique_ptr<TerrainSubdivisionTree>>(subdivideFactor * subdivideFactor);

	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			std::unique_ptr<TerrainSubdivisionTree> terrain = std::unique_ptr<TerrainSubdivisionTree>(
				new TerrainSubdivisionTree(new Terrain(_terrain->getSize())));

			terrain->_parent = this;
			terrain->_subdivisionStage = _subdivisionStage + 1;
			terrain->_x = x;
			terrain->_y = y;

			_subterrain[x * subdivideFactor + y] = std::move(terrain);
		}
	}
}

const TerrainSubdivisionTree & TerrainSubdivisionTree::getSubtreeAt(double x, double y, int stage) const {
	if (stage == 0 || !this->isSubdivided()) {
		const TerrainSubdivisionTree & result = *this;
		return result;
	}

	int xindex = (int)floor(x * _subdivideFactor);
	int yindex = (int)floor(y * _subdivideFactor);

	TerrainSubdivisionTree & subterrain = getSubtree(xindex, yindex);
	double subx = x * _subdivideFactor - xindex;
	double suby = y * _subdivideFactor - yindex;

	return subterrain.getSubtreeAt(subx, suby, stage - 1);
}

TerrainSubdivisionTree & TerrainSubdivisionTree::getSubtree(int xindex, int yindex) const {
	if (xindex >= _subdivideFactor || yindex >= _subdivideFactor || xindex < 0 || yindex < 0) {
		throw std::runtime_error("given coordinates out of bounds");
	}

	return *_subterrain.at(xindex * _subdivideFactor + yindex);
}

void TerrainSubdivisionTree::getLocalPosition(const TerrainSubdivisionTree & child, double globalPosX, double globalPosY, double & localPosX, double & localPosY) const {
	double reductionFactor = 1;
	double offsetX = 0;
	double offsetY = 0;
	const TerrainSubdivisionTree* interTerrain = &child;

	if (interTerrain == this) {
		localPosX = globalPosX;
		localPosY = globalPosY;
	}
	else {
		while (interTerrain != this) {
			reductionFactor /= interTerrain->_parent->_subdivideFactor;
			offsetX = ((double)interTerrain->_x + offsetX) / interTerrain->_parent->_subdivideFactor;
			offsetY = ((double)interTerrain->_y + offsetY) / interTerrain->_parent->_subdivideFactor;

			interTerrain = interTerrain->_parent;
		}

		localPosX = (globalPosX - offsetX) / reductionFactor;
		localPosY = (globalPosY - offsetY) / reductionFactor;
	}
}

