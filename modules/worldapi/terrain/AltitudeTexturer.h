#ifndef WORLD_ALTITUDETEXTURING_H
#define WORLD_ALTITUDETEXTURING_H

#include "core/WorldConfig.h"

#include <random>

#include "core/ColorMap.h"
#include "ITerrainWorker.h"

namespace world {

	class WORLDAPI_EXPORT AltitudeTexturer : public ITerrainWorker {
	public:
		AltitudeTexturer();

		ColorMap &getColorMap();

		void process(Terrain& terrain) override;

		void process(Terrain& terrain, ITerrainWorkerContext &context) override;
	private:
		std::mt19937 _rng;
		ColorMap _colorMap;
	};
}

#endif // WORLD_ALTITUDETEXTURING_H
