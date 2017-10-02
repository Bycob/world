#pragma once

#include <map>

#include <irrlicht.h>

#include <worldapi/maths/MathsHelper.h>
#include <worldapi/world/World.h>
#include <worldapi/Object3D.h>

#include "RenderingModule.h"

class ObjectsManager;
class ChunkNodeHandler;

class ObjectNodeHandler {
public:
	ObjectNodeHandler() {};
	ObjectNodeHandler(ChunkNodeHandler * parent, const Object3D & object);
	virtual ~ObjectNodeHandler();

	void updateObject3D(const Object3D & object);
private:
	ChunkNodeHandler * _parent;
	irr::scene::IMeshSceneNode * _meshNode;
};

class ChunkNodeHandler {
public:
	ChunkNodeHandler() {};
	ChunkNodeHandler(ObjectsManager * manager);
	virtual ~ChunkNodeHandler();

	void updateObject(const Object3D & object);
	void clearObjects();

	irr::scene::ISceneManager * sceneManager() const;

	ObjectsManager * _objectsManager;
private:
	std::map<irr::s64, ObjectNodeHandler> _objects;
};

class ObjectsManager : public RenderingModule {
public:
	ObjectsManager(Application & app, irr::IrrlichtDevice * device);
	virtual ~ObjectsManager();

	void initialize(const World & world) override;
	void update(const World & world) override;

	static irr::scene::SMesh * convertToIrrlichtMesh(const Mesh & mesh, irr::video::IVideoDriver * driver);
private:
	std::map<maths::vec2i, ChunkNodeHandler> _chunks;

	ChunkNodeHandler & getOrCreateNode(const maths::vec2i & pos);
};

