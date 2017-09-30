#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "../Scene.h"

class WORLDAPI_EXPORT ObjectPosition {
public:
	ObjectPosition(const maths::vec3d & pos = maths::vec3d(), int lodMax = 1);
	~ObjectPosition();

	const maths::vec3d & getPosition3D() const;
	int getMaxLOD() const { return _lodMax; }
private:
	maths::vec3d _pos;
	int _lodMax;
};

class WORLDAPI_EXPORT WorldObject {
public:
	WorldObject();
	virtual ~WorldObject();

	void setPosition3D(const maths::vec3d & pos);
	const ObjectPosition & getPosition() const;

	virtual void fillScene(Scene & scene) const;
protected:
	ObjectPosition _position;
};
