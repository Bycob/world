#include "SimpleTreeDecorator.h"

#include <vector>

#include "../maths/MathsHelper.h"

using namespace maths;

SimpleTreeDecorator::SimpleTreeDecorator(int maxTreesPerChunk) 
	: _rng(time(NULL)), _maxTreesPerChunk(maxTreesPerChunk) {

}

SimpleTreeDecorator::~SimpleTreeDecorator() {

}

void SimpleTreeDecorator::setTreeSkelettonGenerator(TreeSkelettonGenerator * generator) {
	_skelettonGenerator = std::unique_ptr<TreeSkelettonGenerator>(generator);
}

void SimpleTreeDecorator::setTreeGenerator(TreeGenerator * generator) {
	_treeGenerator = std::unique_ptr<TreeGenerator>(generator);
}

void SimpleTreeDecorator::decorate(FlatWorld & world, Chunk & chunk) {
	if (chunk.getChunkPosition().getPosition3D().z != 0) return;

	std::vector<vec2d> positions;
	std::uniform_real_distribution<double> distribX(0, 2);
	std::uniform_real_distribution<double> distribY(0, 3);
	
	Ground & ground = world.environment().ground();

	for (int i = 0; i < _maxTreesPerChunk; i++) {
		// On génère une position pour l'arbre
		vec2d position(distribX(_rng), distribY(_rng));

		// On vérifie que les autres arbres ne sont pas trop près
		bool addTree = true;

		for (const vec2d & prevPos : positions) {
			
			if (prevPos.squaredLength(position) < 6) {
				addTree = false;
				break;
			}
		}

		if (!addTree) continue;

		// Génération de l'arbre
		positions.push_back(position);
		std::unique_ptr<TreeSkeletton> skeletton(_skelettonGenerator->generate());
		Tree * tree = _treeGenerator->generate(*skeletton);
		
		// Détermination de l'altitude de l'arbre
		vec3d absolutePosition = chunk.toAbsolutePosition(vec3d(position.x, position.y, 0));
		absolutePosition.z += ground.getAltitudeAt(absolutePosition.x, absolutePosition.y);
		
		tree->setPosition3D(absolutePosition);
		chunk.addObject(tree);
	}
}
