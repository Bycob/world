#include "LODData.h"

#include "math/MathsHelper.h"

namespace world {

	LODData::LODData()
			: _chunkSize({0, 0, 0}) {

	}

	LODData::LODData(const vec3d &chunkSize, double maxResolution)
			: _chunkSize(chunkSize), _maxResolution(maxResolution) {

	}

	LODData::LODData(const LODData &other)
			: _chunkSize(other._chunkSize),
			  _maxResolution(other._maxResolution) {

	}

	LODData::~LODData() {

	}
}
