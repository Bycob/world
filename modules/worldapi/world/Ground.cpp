//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>

#include "Map.h"
#include "../terrain/TerrainGenerator.h"

INIT_TYPE(Ground, "ground", true)

Ground::Ground(const World * world)
        : WorldNode(world, TYPE(Ground)), _unitSize(500) {

}

void Ground::setHeight(float height) {
	_height = height;
}

void Ground::setSeaLevel(float seaLevel) {
	_seaLevel = seaLevel;
}

bool Ground::isTerrainGenerated(int x, int y, int lvl) const {
	return false; // TODO;
}

const Terrain & Ground::getTerrain(int x, int y) const {
	return getTerrain(x, y);
}

Terrain & Ground::getTerrain(int x, int y) {
	throw nullptr;
}





GroundGenerator::GroundGenerator(WorldGenerator * parent)
        : WorldGenNode(parent) {

}

GroundGenerator::GroundGenerator(WorldGenerator * parent, const GroundGenerator & other)
	: WorldGenNode(parent), _generator(other._generator->clone()) {

}

void GroundGenerator::expand(World & world, const maths::vec3d & location) {
	Ground & ground = world.getUniqueNode<Ground>();


}

void GroundGenerator::addRequiredNodes(World &world) const {
	requireUnique<Map>(world);
    requireUnique<Ground>(world);

	world.getUniqueNode<Ground>()._unitSize = world.getUniqueNode<Map>().getUnitsPerPixel();
}

GroundGenerator* GroundGenerator::clone(WorldGenerator * newParent) {
    GroundGenerator * result = new GroundGenerator(newParent, *this);
    return result;
}