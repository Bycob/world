#include "LODData.h"

#include "math/MathsHelper.h"

namespace world {

	LODData::LODData()
			: _chunkSize({0, 0, 0}) {

	}

	LODData::LODData(const vec3d &chunkSize)
			: _chunkSize(chunkSize) {

		const double l = min(chunkSize.x, min(chunkSize.y, chunkSize.z));
		_minDetailSize = l / 128;
	}

	LODData::LODData(const LODData &other)
			: _chunkSize(other._chunkSize),
			  _minDetailSize(other._minDetailSize) {

	}

	LODData::~LODData() {

	}
}
