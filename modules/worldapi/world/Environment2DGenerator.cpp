#include "Environment2DGenerator.h"

#include "../terrain/TerrainManipulator.h"

using namespace maths;

const Environment2DMetadata Environment2DGenerator::DEFAULT_METADATA = { 1500, 4000 };

Environment2DGenerator::Environment2DGenerator() 
	: _metadata(DEFAULT_METADATA) {

	_terrainGenerator = std::make_unique<PerlinTerrainGenerator>(129, 0, 5, 4);

	_mapGenerator = std::make_unique<MapGenerator>(100, 100);
	_mapGenerator->createReliefMapGenerator<CustomWorldRMGenerator>();
}

Environment2DGenerator::Environment2DGenerator(TerrainGenerator * terrainGenerator, MapGenerator * mapGenerator) 
	: _metadata(DEFAULT_METADATA), _terrainGenerator(terrainGenerator), _mapGenerator(mapGenerator) {

}

Environment2DGenerator::Environment2DGenerator(const Environment2DGenerator & generator)
	: _metadata(generator._metadata), _terrainGenerator(generator._terrainGenerator->clone()), _mapGenerator(generator._mapGenerator->clone()) {

}

Environment2DGenerator::~Environment2DGenerator() {

}

void Environment2DGenerator::setTerrainGenerator(TerrainGenerator * generator) {
	_terrainGenerator = std::unique_ptr<TerrainGenerator>(generator);
}

void Environment2DGenerator::setMapGenerator(MapGenerator * generator) {
	_mapGenerator = std::unique_ptr<MapGenerator>(generator);
}

void Environment2DGenerator::generate(FlatWorld & world) {
	world.addFlatWorldExpander(new Environment2DGenerator(*this));
}

void Environment2DGenerator::onAddExpander(FlatWorld & world) {
	Environment2D & env = world.environment();
	env._metadata = _metadata;

	if (_mapGenerator != nullptr) {
		env.setMap(_mapGenerator->generate());
	}

	env.setGround(new Ground());
}

void Environment2DGenerator::expand(FlatWorld & world, const IPointOfView  & from) {
	Environment2D & env = world.environment();

	vec3d location = from.getPosition();

	Map & map = env.map();
	Ground & ground = env.ground();

	if (_terrainGenerator != nullptr) {

		float unit = ground._unitSize;
		float distance = from.getHorizonDistance() + 2 * ground.getUnitSize();

		float tdist = distance / unit;
		float tdist2 = tdist * tdist;
		int centerX = (int)round(location.x / unit);
		int centerY = (int)round(location.y / unit);

		int tx1 = (int)round((location.x - distance) / unit);
		int ty1 = (int)round((location.y - distance) / unit);
		int tx2 = (int)round((location.x + distance) / unit);
		int ty2 = (int)round((location.y + distance) / unit);

		// Creation des terrains
		std::map<vec2i, std::unique_ptr<Terrain>> generated;

		for (int x = tx1; x <= tx2; x++) {
			for (int y = ty1; y <= ty2; y++) {
				int dx = x - centerX;
				int dy = y - centerY;

				if (dx * dx + dy * dy <= tdist2 && !ground.isTerrainGenerated(x, y)) {
					generated[{x, y}] = std::unique_ptr<Terrain>(_terrainGenerator->generate());
				}
			}
		}

		// Fusionnement
		std::map<vec2i, TerrainTile> joinedY;

		// Le joint y doit se faire avant le joint x
		// TODO simplifier l'algorithme suivant ?
		// join y
		for (auto & pair : generated) {
			int x = pair.first.x;
			int y = pair.first.y;
			Terrain& terrain = *pair.second;

			// Détermination du deuxième terrain à joindre
			// Fusion en dessus
			vec2i key1(x, y + 1);
			auto terrain2 = generated.find(key1);

			if (terrain2 != generated.end()) {
				_terrainGenerator->join(terrain, *(*terrain2).second, false, false);
			}
			else if ((terrain2 = ground.terrains().find(key1))
				!= ground.terrains().end()) {

				TerrainTile t2tile = { (*terrain2).second.get(), x, y + 1 };
				if (joinedY.find(key1) == joinedY.end())
					applyMap(t2tile, map, true);
				_terrainGenerator->join(terrain, *t2tile._terrain, false, false);
				joinedY[key1] = t2tile;
			}

			// Fusion en dessous si aucun terrain généré n'est présent
			vec2i key2(x, y - 1);

			if (generated.find(key2) == generated.end() &&
				(terrain2 = ground.terrains().find(key2)) != ground.terrains().end()) {

				TerrainTile t2tile = { (*terrain2).second.get(), x, y - 1 };
				if (joinedY.find({ x, y - 1 }) == joinedY.end())
					applyMap(t2tile, map, true);
				_terrainGenerator->join(*t2tile._terrain, terrain, false, false);
				joinedY[key2] = t2tile;
			}

			joinedY[{x, y}] = { &terrain, x, y };
		}

		// join x
		std::map<vec2i, TerrainTile> joinedX;

		for (auto & pair : joinedY) {
			TerrainTile & tile = pair.second;
			Terrain & terrain = *tile._terrain;
			int x = tile._x;
			int y = tile._y;

			// Fusion x croissants
			vec2i key1(x + 1, y);
			auto terrain2 = joinedY.find(key1);
			auto terrain2bis = ground.terrains().find(key1);

			if (terrain2 != joinedY.end()) {
				_terrainGenerator->join(terrain, *(*terrain2).second._terrain, true, true);
			}
			else if (terrain2bis != ground.terrains().end()) {

				TerrainTile t2tile = { (*terrain2bis).second.get(), x + 1, y };
				if (joinedX.find(key1) == joinedX.end())
					applyMap(t2tile, map, true);
				_terrainGenerator->join(terrain, *t2tile._terrain, true, true);
				joinedX[key1] = t2tile;
			}

			// Fusion x décroissants si aucun terrain généré n'est présent
			vec2i key2(x - 1, y);

			if (joinedY.find(key2) == joinedY.end() &&
				(terrain2bis = ground.terrains().find(key2)) != ground.terrains().end()) {

				TerrainTile t2tile = { (*terrain2bis).second.get(), x - 1, y };
				if (joinedX.find(key2) == joinedX.end())
					applyMap(t2tile, map, true);
				_terrainGenerator->join(*t2tile._terrain, terrain, true, true);
				joinedX[key2] = t2tile;
			}

			joinedX[{tile._x, tile._y}] = tile;
		}

		// Application de la carte
		for (auto & pair : joinedX) {
			applyMap(pair.second, map);
		}

		// Ajout
		for (auto & pair : generated) {
			TerrainTile tile = { pair.second.get(), pair.first.x, pair.first.y };
			ground.terrains()[pair.first] = std::unique_ptr<Terrain>(pair.second.release());
		}
	}
}


void Environment2DGenerator::applyMap(TerrainTile & tile, const Map & map, bool unapply) {
	const double offsetCoef = 0.5;
	const double diffCoef = 1 - offsetCoef;

	const double ratio = _metadata.unitsPerTerrain / _metadata.unitsPerMapPixel;

	Terrain & terrain = *tile._terrain;
	int tX = tile._x;
	int tY = tile._y;
	//std::cout << (unapply ? "unapply " : "apply ") << "to" << tX << ", " << tY << std::endl;

	std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

	int mapOx = map.getSizeX() / 2;
	int mapOy = map.getSizeY() / 2;

	uint32_t size = terrain.getSize();
	arma::mat bufferOffset(size, size);
	arma::mat bufferDiff(size, size);

	for (uint32_t x = 0; x < size; x++) {
		for (uint32_t y = 0; y < size; y++) {
			double mapX = mapOx + (tX + (double)x / size) * ratio;
			double mapY = mapOy + (tY + (double)y / size) * ratio;
			auto data = map.getReliefAt(mapX, mapY);

			double offset = offsetCoef * data.first;
			double diff = data.second * diffCoef;

			if (unapply) {
				bufferOffset(x, y) = -offset;
				bufferDiff(x, y) = diff > std::numeric_limits<double>::epsilon() ? 1 / diff : 0;
			}
			else {
				bufferOffset(x, y) = offset;
				bufferDiff(x, y) = diff;
			}
		}
	}

	if (unapply) {
		manipulator->applyOffset(terrain, bufferOffset);
		manipulator->multiply(terrain, bufferDiff);
	}
	else {
		manipulator->multiply(terrain, bufferDiff);
		manipulator->applyOffset(terrain, bufferOffset);
	}
}
