#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <map>
#include <utility>

#include "../Image.h"
#include "../maths/Perlin.h"
#include "TerrainTexmapBuilder.h"
#include "Terrain.h"
#include "TerrainSubdivisionTree.h"

class WORLDAPI_EXPORT ITerrainGeneratorContext {
public:
    virtual bool neighbourExists(int x, int y) const = 0;
    virtual const Terrain& getNeighbour(int x, int y) const = 0;
};

class WORLDAPI_EXPORT TerrainGenerator {
public:
	TerrainGenerator();
	virtual ~TerrainGenerator();

	Terrain * createTerrain(int size);
	virtual void process(Terrain &terrain) = 0;
    virtual void process(Terrain &terrain, const ITerrainGeneratorContext &context);

	virtual void join(Terrain & terrain1, Terrain & terrain2, bool axisX, bool joinableSides = false) = 0;

	TerrainSubdivisionTree * generateSubdivisions(Terrain & terrain, int subdivideFactor, int subdivisionsCount);
	void generateSubdivisionLevels(TerrainSubdivisionTree & tree, int subdivideFactor, int subdivisionsCount);
	virtual void generateSubdivisionLevel(TerrainSubdivisionTree & tree, int subdivideFactor);

	img::Image generateTexture(const Terrain & terrain, const TerrainTexmapBuilder & builder, const arma::Mat<double> & randomArray) const;
	img::Image generateTexture(const Terrain & terrain, const arma::Cube<double> & map, const arma::Mat<double> & randomArray) const;

	virtual TerrainGenerator * clone() const = 0;
protected :
	/// Indique si la texture doit être générée automatiquement avec le terrain
	bool _generateTexture;

	// ------

	virtual void generateSubdivision(TerrainSubdivisionTree & terrain, int x, int y) = 0;
};

class WORLDAPI_EXPORT PerlinTerrainGenerator : public TerrainGenerator {
public :
	PerlinTerrainGenerator(int offset = 0, int octaveCount = 5, float frequency = 8, float persistence = 0.5);
	~PerlinTerrainGenerator() override;

	void setFrequency(float frequency) {_frequency = frequency;}

	void process(Terrain &terrain) override;
    void process(Terrain &terrain, const ITerrainGeneratorContext &context) override;

	void join(Terrain & terrain1, Terrain & terrain2, bool axisX, bool joinableSides) override;

	void generateSubdivisionLevel(TerrainSubdivisionTree & tree, int subdivideFactor) override;

	TerrainGenerator * clone() const override;
protected :
	void generateSubdivision(TerrainSubdivisionTree & tree, int x, int y) override;
private :
	Perlin _perlin;
	mutable std::map<std::pair<int, int>, arma::Mat<double>> _buffer;

	int _offset;
	int _octaveCount;
	float _frequency;
	float _persistence;
	double _subdivNoiseRatio = 0.1;

	// -------
	arma::mat & getBuf(int x, int y) const;

	template <typename... Args>
	arma::Mat<double> & createInBuf(int x, int y, Args&&... args) const {
		std::pair<int, int> index = std::make_pair(x, y);
		_buffer[index] = arma::Mat<double>(args...);
		return _buffer[index];
    }

	void adaptFromBuffer(TerrainSubdivisionTree & terrain, int xsub, int ysub) const;
};
