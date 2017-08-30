#include "Environment2DGenerator.h"

#include "../terrain/TerrainManipulator.h"

using namespace maths;

Environment2DGenerator::Environment2DGenerator() 
	: _terrainGenerator(new PerlinTerrainGenerator(129, 0, 5, 4)), _mapGenerator(new MapGenerator(100, 100)) {

}

Environment2DGenerator::Environment2DGenerator(TerrainGenerator * terrainGenerator, MapGenerator * mapGenerator) 
	: _terrainGenerator(terrainGenerator), _mapGenerator(mapGenerator) {

}

Environment2DGenerator::Environment2DGenerator(const Environment2DGenerator & generator)
	: _terrainGenerator(generator._terrainGenerator->clone()), _mapGenerator(generator._mapGenerator->clone()) {

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
	Environment2D & env = world.getEnvironment();

	if (_mapGenerator != nullptr) {
		env.setMap(_mapGenerator->generate());
	}

	env.setGround(new Ground());

	world.addFlatWorldExpander(new Environment2DGenerator(*this));
}

void Environment2DGenerator::expand(FlatWorld & world, const IPointOfView  & from) {
	Environment2D & env = world.getEnvironment();

	vec3d location = from.getPosition();

	Map & map = env.getMap();
	Ground & ground = env.getGround();

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
		std::map<std::pair<int, int>, std::unique_ptr<Terrain>> generated;

		for (int x = tx1; x <= tx2; x++) {
			for (int y = ty1; y <= ty2; y++) {
				int dx = x - centerX;
				int dy = y - centerY;

				if (dx * dx + dy * dy <= tdist2 && !ground.isTerrainGenerated(x, y)) {
					generated[std::pair<int, int>(x, y)] = std::unique_ptr<Terrain>(_terrainGenerator->generate());
				}
			}
		}

		// Fusionnement
		std::map<std::pair<int, int>, TerrainTile> joinedY;

		// Le joint y doit se faire avant le joint x
		// TODO simplifier l'algorithme suivant ?
		// join y
		for (auto & pair : generated) {
			int x = pair.first.first;
			int y = pair.first.second;
			Terrain& terrain = *pair.second;

			// Détermination du deuxième terrain à joindre
			// Fusion en dessus
			std::pair<int, int> key1(x, y + 1);
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
			std::pair<int, int> key2(x, y - 1);

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
		std::map<std::pair<int, int>, TerrainTile> joinedX;

		for (auto & pair : joinedY) {
			TerrainTile & tile = pair.second;
			Terrain & terrain = *tile._terrain;
			int x = tile._x;
			int y = tile._y;

			// Fusion x croissants
			std::pair<int, int> key1(x + 1, y);
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
			std::pair<int, int> key2(x - 1, y);

			if (joinedY.find(key2) == joinedY.end() &&
				(terrain2bis = ground.terrains().find(key2)) != ground.terrains().end()) {

				TerrainTile t2tile = { (*terrain2bis).second.get(), x - 1, y };
				if (joinedX.find(key2) == joinedX.end())
					applyMap(t2tile, map, true);
				else {
					std::cout << "JE T'AI TROUVE SALE BUG" << std::endl;
				}
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
			TerrainTile tile = { pair.second.get(), pair.first.first, pair.first.second };
			ground.terrains()[pair.first] = std::unique_ptr<Terrain>(pair.second.release());
		}
	}
}


void Environment2DGenerator::applyMap(TerrainTile & tile, const Map & map, bool unapply) {
	const double offsetCoef = 0.5;
	const double diffCoef = 1 - offsetCoef;

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
			double mapX = tX + mapOx + (double)x / size;
			double mapY = tY + mapOy + (double)y / size;
			auto data = map.getReliefAt(mapX, mapY);

			if (unapply) {
				bufferOffset(x, y) = -offsetCoef * data.first;
				bufferDiff(x, y) = 1 / (data.second * diffCoef);
			}
			else {
				bufferOffset(x, y) = offsetCoef * data.first;
				bufferDiff(x, y) = diffCoef * data.second;
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
