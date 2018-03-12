#include "SimpleTreeDecorator.h"

#include <vector>

#include "../maths/MathsHelper.h"

using namespace maths;

SimpleTreeDecorator::SimpleTreeDecorator(int maxTreesPerChunk) 
	: _rng(time(NULL)), _maxTreesPerChunk(maxTreesPerChunk),
	  _skelettonGenerator(std::make_unique<TreeSkelettonGenerator>()),
	  _treeGenerator(std::make_unique<TreeGenerator>(12, 0.25, 0.2, 4)) {

	_skelettonGenerator->setConstantMaxForkingLevel(2);
	_skelettonGenerator->setConstantForkingCount(3);
}

SimpleTreeDecorator::SimpleTreeDecorator(const SimpleTreeDecorator &other)
	: _rng(other._rng), _maxTreesPerChunk(other._maxTreesPerChunk),
	  _skelettonGenerator(other._skelettonGenerator->clone()),
	  _treeGenerator(other._treeGenerator->clone()) {

}

SimpleTreeDecorator::~SimpleTreeDecorator() {

}

void SimpleTreeDecorator::setTreeSkelettonGenerator(TreeSkelettonGenerator * generator) {
	_skelettonGenerator = std::unique_ptr<TreeSkelettonGenerator>(generator);
}

void SimpleTreeDecorator::setTreeGenerator(TreeGenerator * generator) {
	_treeGenerator = std::unique_ptr<TreeGenerator>(generator);
}

void SimpleTreeDecorator::decorate(FlatWorld & world, ChunkNode & chunkNode) {
	Chunk &chunk = chunkNode._chunk;

	if (chunk.getMinDetailSize() > 0.5 || chunk.getMaxDetailSize() <= 0.5)
		return;

	vec3d chunkSize = chunk.getSize();

	std::vector<vec2d> positions;
	std::uniform_real_distribution<double> distribX(0, chunkSize.x);
	std::uniform_real_distribution<double> distribY(0, chunkSize.y);
	
	Ground & ground = world.ground();

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
		// FIXME Actuellement des arbres pop dans tous les chunks verticalement :/
		// FIXME Attention, ne fonctionne plus si on active le lod
		vec3d pos3D(position.x, position.y,
					ground.observeAltitudeAt(position.x, position.y) - chunk.getOffset().z);

		tree->setPosition3D(pos3D);
		chunk.addObject(tree);
		std::cout << pos3D << std::endl;
	}
}
