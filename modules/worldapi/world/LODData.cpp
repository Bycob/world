#include "LODData.h"

using namespace maths;

LODData::LODData()
	: _chunkSize({0, 0, 0}){

}

LODData::LODData(const vec3d & chunkSize) 
	: _chunkSize(chunkSize) {

	const double l = _chunkSize.norm();
	_minDetailSize = l / 100;
}

LODData::LODData(const LODData & other) 
	: _chunkSize(other._chunkSize),
	  _minDetailSize(other._minDetailSize) {

}

LODData::~LODData() {

}
