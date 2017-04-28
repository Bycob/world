//
// Created by louis on 20/04/17.
//

#include "WorldGenerator.h"


WorldGenerator* WorldGenerator::defaultGenerator() {
    WorldGenerator * result = new WorldGenerator();

    return result;
}

WorldGenerator::WorldGenerator() {

}

WorldGenerator::WorldGenerator(const WorldGenerator &other) {

}

WorldGenerator::~WorldGenerator() {

}

World* WorldGenerator::createWorld() {
    return new World(*this);
}

void WorldGenerator::init(World &world) {

}