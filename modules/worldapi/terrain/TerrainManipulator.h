#pragma once

#include "core/WorldConfig.h"

#include "Terrain.h"

namespace world {
	// TODO Change to static method, rename to TerrainOps

	class WORLDAPI_EXPORT ITerrainManipulator {
	public:
		static ITerrainManipulator *createManipulator();

		virtual void applyOffset(Terrain &terrain, const arma::mat &offset) const = 0;

		virtual void multiply(Terrain &terrain, const arma::mat &factor) const = 0;

		virtual void multiply(Terrain &terrain, double factor) const = 0;
	};

	class TerrainManipulator : public ITerrainManipulator {
	public:
		void applyOffset(Terrain &terrain, const arma::mat &offset) const override;

		void multiply(Terrain &terrain, const arma::mat &factor) const override;

		void multiply(Terrain &terrain, double factor) const override;
	};
}
