#include "TerrainSubdivisionTree.h"

#include <worldapi/maths/MathsHelper.h>
#include <worldapi/maths/Interpolation.h>

namespace world {

	TerrainSubdivisionTree::TerrainSubdivisionTree(const Terrain &terrain) :
			_terrain(std::make_unique<Terrain>(terrain)) {

	}

	TerrainSubdivisionTree::TerrainSubdivisionTree(Terrain &&terrain) :
			_terrain(std::make_unique<Terrain>(terrain)) {

	}

	TerrainSubdivisionTree::TerrainSubdivisionTree(const TerrainSubdivisionTree &terrain)
			: _terrain(std::make_unique<Terrain>(*terrain._terrain)),
			  _subdivisionStage(terrain._subdivisionStage),
			  _noisePart(terrain._noisePart),
			  _x(terrain._x),
			  _y(terrain._y),
			  _parent(nullptr),
			  _subdivideFactor(terrain._subdivideFactor) {

		for (auto &ptr : terrain._subtrees) {
			_subtrees.emplace_back(new TerrainSubdivisionTree(*ptr));
		}
		for (auto &ptr : _subtrees) {
			ptr->_parent = this;
		}
	}

	TerrainSubdivisionTree::TerrainSubdivisionTree(TerrainSubdivisionTree &&terrain)
			: _terrain(std::move(terrain._terrain)),
			  _subdivisionStage(terrain._subdivisionStage),
			  _noisePart(terrain._noisePart),
			  _x(terrain._x),
			  _y(terrain._y),
			  _parent(nullptr),
			  _subdivideFactor(terrain._subdivideFactor) {

		for (auto &ptr : terrain._subtrees) {
			_subtrees.push_back(std::move(ptr));
		}
		for (auto &ptr : _subtrees) {
			ptr->_parent = this;
		}
	}

	TerrainSubdivisionTree::~TerrainSubdivisionTree() {

	}

	double TerrainSubdivisionTree::getZ(double x, double y, int stage) const {
		const TerrainSubdivisionTree &subtree = getSubtreeAt(x, y, stage);

		double posXd, posYd;
		getLocalPosition(subtree, x, y, posXd, posYd);

		return subtree._terrain->getRawHeight(posXd, posYd);
	}

	double TerrainSubdivisionTree::getZInterpolated(double x, double y, int stage) const {
		const TerrainSubdivisionTree &subtree = getSubtreeAt(x, y, stage);

		// Positions flottantes sur le subterrain selectionné
		double posXd, posYd;
		getLocalPosition(subtree, x, y, posXd, posYd);

		return subtree._terrain->getExactHeightAt(posXd, posYd);
	}

	bool TerrainSubdivisionTree::isSubdivided() const {
		return _subdivideFactor != 0;
	}

	int TerrainSubdivisionTree::getLevelDiff(const TerrainSubdivisionTree &terrain) const {
		return _subdivisionStage - terrain._subdivisionStage;
	}

	void TerrainSubdivisionTree::subdivide(int subdivideFactor) {
		_subdivideFactor = subdivideFactor;
		_subtrees = std::vector<std::unique_ptr<TerrainSubdivisionTree>>(subdivideFactor * subdivideFactor);

		for (int x = 0; x < subdivideFactor; x++) {
			for (int y = 0; y < subdivideFactor; y++) {
				std::unique_ptr<TerrainSubdivisionTree> subtree = std::unique_ptr<TerrainSubdivisionTree>(
						new TerrainSubdivisionTree(new Terrain(_terrain->getSize())));

				subtree->_parent = this;
				subtree->_subdivisionStage = _subdivisionStage + 1;
				subtree->_x = x;
				subtree->_y = y;

				_subtrees[x * subdivideFactor + y] = std::move(subtree);
			}
		}
	}

	const TerrainSubdivisionTree &TerrainSubdivisionTree::getSubtreeAt(double x, double y, int stage) const {
		if (stage <= 0 || !this->isSubdivided()) {
			const TerrainSubdivisionTree &result = *this;
			return result;
		}

		int xindex = (int) floor(x * _subdivideFactor);
		int yindex = (int) floor(y * _subdivideFactor);

		TerrainSubdivisionTree &subterrain = getSubtree(xindex, yindex);
		double subx = x * _subdivideFactor - xindex;
		double suby = y * _subdivideFactor - yindex;

		return subterrain.getSubtreeAt(subx, suby, stage - 1);
	}

	TerrainSubdivisionTree &TerrainSubdivisionTree::getSubtree(int xindex, int yindex) const {
		if (xindex >= _subdivideFactor || yindex >= _subdivideFactor || xindex < 0 || yindex < 0) {
			throw std::runtime_error("given coordinates out of bounds");
		}

		return *_subtrees.at(xindex * _subdivideFactor + yindex);
	}

	Mesh *TerrainSubdivisionTree::convertToSubmesh(float rootSizeX, float rootSizeY, float rootSizeZ) const {
		float sizeX = rootSizeX;
		float sizeY = rootSizeY;
		float sizeZ = rootSizeZ;
		float offsetX = 0;
		float offsetY = 0;
		float offsetZ = 0;

		const TerrainSubdivisionTree *child = this;

		// Calcul de la taille de ce terrain à partir de la taille du parent, ainsi que de l'offset à appliquer.
		while (child->_parent != nullptr) {
			sizeX /= child->_parent->_subdivideFactor;
			sizeY /= child->_parent->_subdivideFactor;

			offsetX = (offsetX + child->_x) / child->_parent->_subdivideFactor;
			offsetY = (offsetY + child->_y) / child->_parent->_subdivideFactor;

			child = child->_parent;
		}

		offsetX *= rootSizeX;
		offsetY *= rootSizeY;

		return _terrain->createMesh(offsetX - rootSizeX / 2, offsetY - rootSizeY / 2, offsetZ, sizeX, sizeY, sizeZ);
	}

	void
	TerrainSubdivisionTree::getLocalPosition(const TerrainSubdivisionTree &child, double globalPosX, double globalPosY,
											 double &localPosX, double &localPosY) const {
		double reductionFactor = 1;
		double offsetX = 0;
		double offsetY = 0;
		const TerrainSubdivisionTree *interTerrain = &child;

		if (interTerrain == this) {
			localPosX = globalPosX;
			localPosY = globalPosY;
		} else {
			while (interTerrain != this) {
				reductionFactor /= interTerrain->_parent->_subdivideFactor;
				offsetX = ((double) interTerrain->_x + offsetX) / interTerrain->_parent->_subdivideFactor;
				offsetY = ((double) interTerrain->_y + offsetY) / interTerrain->_parent->_subdivideFactor;

				interTerrain = interTerrain->_parent;
			}

			localPosX = (globalPosX - offsetX) / reductionFactor;
			localPosY = (globalPosY - offsetY) / reductionFactor;
		}
	}
}
