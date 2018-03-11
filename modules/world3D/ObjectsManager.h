#pragma once

#include <map>

#include <irrlicht.h>

#include <worldapi/maths/MathsHelper.h>
#include <worldapi/world/World.h>
#include <worldapi/Object3D.h>

#include "RenderingModule.h"

class ObjectsManager;

class ObjectNodeHandler {
public:
	ObjectNodeHandler(ObjectsManager& objectsManager, const Object3D & object);
	virtual ~ObjectNodeHandler();

	void updateObject3D(const Object3D & object);
private:
	ObjectsManager& _objManager;

	irr::scene::IMeshSceneNode * _meshNode;
};

class ObjectsManager : public RenderingModule {
public:
	ObjectsManager(Application & app, irr::IrrlichtDevice * device);
	virtual ~ObjectsManager();

	void initialize(const FlatWorldCollector &collector) override;
	void update(const FlatWorldCollector &collector) override;

	static irr::scene::SMesh * convertToIrrlichtMesh(const Mesh & mesh, irr::video::IVideoDriver * driver);
private:
	std::map<WorldCollector::ObjectKey, std::unique_ptr<ObjectNodeHandler>> _objects;
};

