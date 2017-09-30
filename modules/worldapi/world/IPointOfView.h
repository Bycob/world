#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"

class WORLDAPI_EXPORT IPointOfView {
public:
	virtual ~IPointOfView() = default;

	// TODO supprimer ces deux fonctions une fois la refonte terminée.
	virtual maths::vec3d getPosition() const = 0;
	virtual double getHorizonDistance() const = 0;

	/** Indique quelle est la taille de détail minimale visible à 
	l'endroit indiqué. */
	virtual double getResolutionAt(const maths::vec3d & position) const = 0;
};

class WORLDAPI_EXPORT PointOfView : public IPointOfView {
public:
	PointOfView(maths::vec3d position, float horizon)
		: _pos(position), _horizon(horizon) {}

	maths::vec3d getPosition() const override { return _pos; }
	double getHorizonDistance() const override { return _horizon; }

	double getResolutionAt(const maths::vec3d & position) const override;

	maths::vec3d _pos;
	double _horizon;
};

