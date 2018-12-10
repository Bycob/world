#include "RenderingModule.h"

#include "Application.h"

RenderingModule::RenderingModule(Application & app, irr::IrrlichtDevice * device)
	: _app(app),
	_driver(device->getVideoDriver()),
	_sceneManager(device->getSceneManager()),
	_fileSystem(device->getFileSystem()) {

}


RenderingModule::~RenderingModule() {

}
