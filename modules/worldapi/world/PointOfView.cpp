#include "IPointOfView.h"

double PointOfView::getResolutionAt(const maths::vec3d & position) const {
	return position.squaredLength(_pos) < _horizon * _horizon ? 1 : -1;
}