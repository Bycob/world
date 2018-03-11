#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "Chunk.h"

class WORLDAPI_EXPORT WorldObject {
public:
	WorldObject();
	virtual ~WorldObject();

	void setPosition3D(const maths::vec3d & pos);
	maths::vec3d getPosition3D() const {return _position;}

protected:
	maths::vec3d _position;
};
