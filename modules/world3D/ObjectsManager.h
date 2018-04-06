#pragma once

#include <map>
#include <chrono>

#include <irrlicht.h>

#include <worldcore.h>
#include <worldflat.h>

#include "RenderingModule.h"

class ObjectsManager;

class ObjectNodeHandler {
public:
	ObjectNodeHandler(ObjectsManager& objectsManager, const world::CollectorItem &provider);
	virtual ~ObjectNodeHandler();

	bool removeTag = false;
private:
	ObjectsManager& _objManager;

	irr::scene::IMeshSceneNode * _meshNode;
	std::vector<std::string> _usedTextures;

	void setTexture(int id, const std::string &path, const world::CollectorItem &provider);
	void setMaterial(const world::Material &material, const world::CollectorItem &provider);
	void updateObject3D(const world::Object3D & object);
};

class ObjectsManager : public RenderingModule {
public:
	ObjectsManager(Application & app, irr::IrrlichtDevice * device);
	~ObjectsManager() override;

	void initialize(world::FlatWorldCollector &collector) override;
	void update(world::FlatWorldCollector &collector) override;

	void addTextureUser(const std::string &texId);
	void removeTextureUser(const std::string &texId);

	static irr::scene::SMesh * convertToIrrlichtMesh(const world::Mesh & mesh, irr::video::IVideoDriver * driver);
private:
	std::map<world::Collector::ItemKey, std::unique_ptr<ObjectNodeHandler>> _objects;
	std::map<std::string, int> _loadedTextures;

	// Debug variables
	bool _dbgOn = false;
	int _dbgAdded = 0;
	int _dbgRemoved = 0;
	long _elapsedTime = 0;
};

