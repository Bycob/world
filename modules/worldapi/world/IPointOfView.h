#pragma once

#include "../maths/mathshelper.h"

class IPointOfView {
public:
	virtual ~IPointOfView() = default;

	virtual maths::vec3d getPosition() const = 0;
	virtual double getHorizonDistance() const = 0;
};

class PointOfView : public IPointOfView{
public:
	PointOfView(maths::vec3d position, double horizon)
		: _pos(position), _horizon(horizon) {}

	maths::vec3d getPosition() const override { return _pos; }
	double getHorizonDistance() const override { return _horizon; }

	maths::vec3d _pos;
	double _horizon;
};

