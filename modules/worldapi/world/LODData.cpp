#include "LODData.h"

using namespace maths;

LODData::LODData(const vec3d & chunkSize) 
	: _chunkSize(chunkSize) {

}

LODData::LODData(const LODData & other) 
	: _chunkSize(other._chunkSize) {

}

LODData::~LODData() {

}
