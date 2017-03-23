#pragma once

#include "../worldapidef.h"
#include "../Image.h"
#include "../GenBase.h"
#include "TerrainTexmapBuilder.h"
#include "terrain.h"

#define DEFAULT_TERRAIN_SIZE 257

class WORLDAPI_EXPORT TerrainGenerator : public GenBase<Terrain> {
public:
	TerrainGenerator(int size = DEFAULT_TERRAIN_SIZE);
	virtual ~TerrainGenerator();

	// ACCESSEURS
	void setSubdivisionsCount(int subdivisionCount);
	void setSize(int size);

	img::Image generateTexture(const Terrain & terrain, const TerrainTexmapBuilder & builder, const Mat<double> & randomArray) const;
	img::Image generateTexture(const Terrain & terrain, const arma::Cube<double> & map, const Mat<double> & randomArray) const;

	void generateSubdivisions(Terrain & terrain, int subdivideFactor, int subdivisionsCount);
protected :
	virtual void generateSubdivision(Terrain & terrain, int x, int y) const = 0;
	
	int _size;

	bool _generateTexture;
	
	int _subdivisionCount = 0;
};

class WORLDAPI_EXPORT PerlinTerrainGenerator : public TerrainGenerator {
public :
	PerlinTerrainGenerator(int size = DEFAULT_TERRAIN_SIZE, int offset = 0, int octaveCount = 5, double frequency = 8, double persistence = 0.5);
	virtual ~PerlinTerrainGenerator();

	virtual std::unique_ptr<Terrain> generate() const;

protected :
	virtual void generateSubdivision(Terrain & terrain, int x, int y) const;
private :
	int _offset;
	double _octaveCount;
	double _frequency;
	double _persistence;
	double _subdivNoiseRatio = 0.05;
};
