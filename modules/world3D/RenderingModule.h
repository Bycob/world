#pragma once

#include <irrlicht.h>

class Application;
class World;

class RenderingModule {
public:
	RenderingModule(Application & app, irr::IrrlichtDevice * device);
	virtual ~RenderingModule();

	virtual void initialize(const World & world) = 0;
	virtual void update(const World & world) = 0;

	Application & _app;

	irr::scene::ISceneManager * _sceneManager;
	irr::video::IVideoDriver *_driver;
	irr::io::IFileSystem * _fileSystem;
};

