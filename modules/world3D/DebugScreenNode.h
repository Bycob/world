#pragma once

#include <irrlicht.h>

#include <worldapi/maths/mathshelper.h>

class Application;

class DebugScreenNode : public irr::scene::ISceneNode {
public:
	DebugScreenNode(Application & app, irr::IrrlichtDevice * device, irr::scene::ISceneNode* parent, irr::s32 id);
	virtual ~DebugScreenNode();
	
	void updateInfos();
	void setVisible(bool visible);

	virtual void OnRegisterSceneNode();
	virtual void render();

	virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const { return _box; }
	virtual irr::u32 getMaterialCount() const { return 1; }
	virtual irr::video::SMaterial& getMaterial(irr::u32 i) { return _material; }
private:
	Application & _app;
	irr::IrrlichtDevice * _device;

	irr::core::aabbox3d<irr::f32> _box;
	irr::video::SMaterial _material;

	bool _visible;
	maths::vec3d _userPos;
};

