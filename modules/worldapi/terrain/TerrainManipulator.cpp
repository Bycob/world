#include "TerrainManipulator.h"

#include <armadillo/armadillo>

ITerrainManipulator * ITerrainManipulator::createManipulator() {
	return new TerrainManipulator();
}

void TerrainManipulator::setZ(Terrain & terrain, double x, double y, double value, int stage, bool applyToChildren) const {
	if (stage == 0) {
		auto pos = terrain.getPixelPos(x, y);
		double oldValue = terrain._array(pos.x, pos.y);
		terrain._array(pos.x, pos.y) = value;

		if (applyToChildren && terrain.isSubdivided()) {
			// TODO Apply to children
		}
	}
	else {

	}
}

void TerrainManipulator::applyOffset(Terrain & terrain, const arma::mat & offset, bool applyToChildren) const {
	if (offset.n_rows != terrain._array.n_rows || offset.n_cols != terrain._array.n_cols) {
		throw std::runtime_error("TerrainManipulator::applyOffset : bad matrix dimensions");
	}

	terrain._array += offset;

	if (applyToChildren && terrain.isSubdivided()) {
		// TODO, TODO...
	}
}

void TerrainManipulator::multiply(Terrain & terrain, const arma::mat & factor, bool applyToChildren) const {
	if (factor.n_rows != terrain._array.n_rows || factor.n_cols != terrain._array.n_cols) {
		throw std::runtime_error("TerrainManipulator::multiply : bad matrix dimensions");
	}

	terrain._array %= factor;

	if (applyToChildren && terrain.isSubdivided()) {

	}
}

