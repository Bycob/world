#pragma once

#include <irrlicht.h>

#include <world/core.h>
#include <world/flat.h>

class Application;

class RenderingModule {
public:
	RenderingModule(Application & app, irr::IrrlichtDevice * device);
	virtual ~RenderingModule();

	/* This method is called to initialize or reinitialize the module with
	 * the given collector. */
	virtual void initialize(world::Collector &collector) = 0;
	virtual void update(world::Collector &collector) = 0;

	Application & _app;

	irr::video::IVideoDriver *_driver;
	irr::scene::ISceneManager * _sceneManager;
	irr::io::IFileSystem * _fileSystem;
};

