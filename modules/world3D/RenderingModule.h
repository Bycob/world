#pragma once

#include <irrlicht.h>

#include <worldcore.h>
#include <worldflat.h>

class Application;

class RenderingModule {
public:
	RenderingModule(Application & app, irr::IrrlichtDevice * device);
	virtual ~RenderingModule();

	virtual void initialize(world::FlatWorldCollector &collector) = 0;
	virtual void update(world::FlatWorldCollector &collector) = 0;

	Application & _app;

	irr::scene::ISceneManager * _sceneManager;
	irr::video::IVideoDriver *_driver;
	irr::io::IFileSystem * _fileSystem;
};

