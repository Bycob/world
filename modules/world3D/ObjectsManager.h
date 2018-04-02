#pragma once

#include <map>
#include <chrono>

#include <irrlicht.h>

#include <worldapi/maths/MathsHelper.h>
#include <worldapi/world/World.h>
#include <worldapi/assets/Material.h>
#include <worldapi/assets/Object3D.h>

#include "RenderingModule.h"

class ObjectsManager;

class ObjectNodeHandler {
public:
	ObjectNodeHandler(ObjectsManager& objectsManager, const world::Object3D & object);
	virtual ~ObjectNodeHandler();

	void setMaterial(const world::Material &material);
	void updateObject3D(const world::Object3D & object);

	bool removeTag = false;
private:
	ObjectsManager& _objManager;

	irr::scene::IMeshSceneNode * _meshNode;
};

class ObjectsManager : public RenderingModule {
public:
	ObjectsManager(Application & app, irr::IrrlichtDevice * device);
	~ObjectsManager() override;

	void initialize(world::FlatWorldCollector &collector) override;
	void update(world::FlatWorldCollector &collector) override;

	static irr::scene::SMesh * convertToIrrlichtMesh(const world::Mesh & mesh, irr::video::IVideoDriver * driver);
private:
	std::map<world::Collector::ItemKey, std::unique_ptr<ObjectNodeHandler>> _objects;

	// Debug variables
	bool _dbgOn = false;
	int _dbgAdded = 0;
	int _dbgRemoved = 0;
	long _elapsedTime = 0;
};

