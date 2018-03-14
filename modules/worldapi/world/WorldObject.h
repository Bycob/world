#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "ChunkSystem.h"

class CollectorObject;

class WORLDAPI_EXPORT WorldObject {
public:
	WorldObject();
	virtual ~WorldObject();

	void setPosition3D(const maths::vec3d & pos);
	maths::vec3d getPosition3D() const {return _position;}

	virtual void collectWholeObject(WorldZone &zone, CollectorObject &collector);
protected:
	maths::vec3d _position;
};
