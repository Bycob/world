#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "Chunk.h"

class WORLDAPI_EXPORT ObjectPosition {
public:
	ObjectPosition(const maths::vec3d & offset = maths::vec3d());
	~ObjectPosition();

	const maths::vec3d & getPosition3D() const;
private:
	maths::vec3d _pos;
};

class WORLDAPI_EXPORT WorldObject {
public:
	WorldObject();
	virtual ~WorldObject();

	void setPosition3D(const maths::vec3d & pos);
	const ObjectPosition & getPosition() const;

protected:
	ObjectPosition _position;
};
