#include "Environment2D.h"

Environment2D::Environment2D() :
	_ground(std::make_unique<Ground>()),
	_map(std::make_unique<Map>()) {

}

Environment2D::~Environment2D() {

}

void Environment2D::setMap(Map * map) {
	_map = std::unique_ptr<Map>(map);
}

void Environment2D::setGround(Ground * ground) {
	_ground = std::unique_ptr<Ground>(ground);
}

void Environment2D::setMetadata(const Environment2DMetadata & metadata) {
	_metadata = metadata;
}
