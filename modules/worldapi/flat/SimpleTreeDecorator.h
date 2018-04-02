#pragma once

#include "core/WorldConfig.h"

#include <random>

#include "FlatWorld.h"
#include "tree/TreeGenerator.h"
#include "tree/TreeSkelettonGenerator.h"

// TODO relocate in "tree/" ?
namespace world {

	class WORLDAPI_EXPORT SimpleTreeDecorator : public FlatWorldDecorator {
	public:
		SimpleTreeDecorator(int maxTreesPerChunk = 1);

		SimpleTreeDecorator(const SimpleTreeDecorator &other);

		virtual ~SimpleTreeDecorator();

		void setTreeSkelettonGenerator(TreeSkelettonGenerator *generator);

		void setTreeGenerator(TreeGenerator *generator);

		void decorate(FlatWorld &world, WorldZone &zone) override;

	private:
		int _maxTreesPerChunk;

		std::mt19937 _rng;

		std::unique_ptr<TreeSkelettonGenerator> _skelettonGenerator;
		std::unique_ptr<TreeGenerator> _treeGenerator;
	};
}
