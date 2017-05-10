#pragma once

#include <worldapi/worldapidef.h>

#include "terrain.h"

class WORLDAPI_EXPORT ITerrainManipulator {
public:
	static ITerrainManipulator * createManipulator();

	virtual void setZ(Terrain & terrain, double x, double y, double value, int lvl = 0, bool applyToChild = false) const = 0;
	virtual void applyOffset(Terrain & terrain, const arma::mat & offset, bool applyToChildren = false) const = 0;
	virtual void multiply(Terrain & terrain, const arma::mat & factor, bool applyToChildren = false) const = 0;
};

class TerrainManipulator : public ITerrainManipulator {
public:
	void setZ(Terrain & terrain, double x, double y, double value, int stage, bool applyToChild) const override;
	void applyOffset(Terrain & terrain, const arma::mat & offset, bool applyToChildren) const override;
	void multiply(Terrain & terrain, const arma::mat & factor, bool applyToChildren) const override;
};