#include "DebugScreenNode.h"

#include <string>

#include "Application.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;

DebugScreenNode::DebugScreenNode(Application & app, IrrlichtDevice *device, ISceneNode* parent, s32 id)
	: ISceneNode(parent, device->getSceneManager(), id), _app(app), _device(device) {

	_material.ColorMaterial = ECM_NONE;
	_material.AmbientColor.set(0, 0, 0, 0);
	
	setAutomaticCulling(EAC_OFF);
}

DebugScreenNode::~DebugScreenNode() {

}

void DebugScreenNode::updateInfos() {
	_userPos = _app.getUserPosition();
}

void DebugScreenNode::OnRegisterSceneNode() {
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void DebugScreenNode::render() {
	gui::IGUIFont * font = _device->getGUIEnvironment()->getBuiltInFont();

	std::string str("x :");
	str += std::to_string(_userPos.x);
	str += "\ny :";
	str += std::to_string(_userPos.y);
	str += "\nz : ";
	str += std::to_string(_userPos.z);

	font->draw(str.c_str() , rect<s32>(10, 10, 4000, 400), SColor(255, 255, 255, 255));
}

void DebugScreenNode::setVisible(bool visible) {
	
	if (visible != _visible) {
		// qqc ?

		_visible = visible;
	}
}
