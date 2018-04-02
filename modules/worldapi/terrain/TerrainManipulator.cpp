#include "TerrainManipulator.h"

#include <armadillo/armadillo>

namespace world {

	ITerrainManipulator *ITerrainManipulator::createManipulator() {
		return new TerrainManipulator();
	}

	void TerrainManipulator::applyOffset(Terrain &terrain, const arma::mat &offset) const {
		if (offset.n_rows != terrain._array.n_rows || offset.n_cols != terrain._array.n_cols) {
			throw std::runtime_error("TerrainManipulator::applyOffset : bad matrix dimensions");
		}

		terrain._array += offset;
	}

	void TerrainManipulator::multiply(Terrain &terrain, const arma::mat &factor) const {
		if (factor.n_rows != terrain._array.n_rows || factor.n_cols != terrain._array.n_cols) {
			throw std::runtime_error("TerrainManipulator::multiply : bad matrix dimensions");
		}

		terrain._array %= factor;
	}

	void TerrainManipulator::multiply(Terrain &terrain, double factor) const {
		terrain._array *= factor;
	}
}
