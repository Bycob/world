#pragma once

#include <worldapi/worldapidef.h>

#include "Terrain.h"

namespace world {
	class WORLDAPI_EXPORT ITerrainManipulator {
	public:
		static ITerrainManipulator *createManipulator();

		virtual void setZ(Terrain &terrain, double x, double y, double value, int lvl = 0) const = 0;

		virtual void applyOffset(Terrain &terrain, const arma::mat &offset) const = 0;

		virtual void multiply(Terrain &terrain, const arma::mat &factor) const = 0;

		virtual void multiply(Terrain &terrain, double factor) const = 0;
	};

	class TerrainManipulator : public ITerrainManipulator {
	public:
		void setZ(Terrain &terrain, double x, double y, double value, int stage) const override;

		void applyOffset(Terrain &terrain, const arma::mat &offset) const override;

		void multiply(Terrain &terrain, const arma::mat &factor) const override;

		void multiply(Terrain &terrain, double factor) const override;
	};
}
