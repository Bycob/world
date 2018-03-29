#include "ObjectsManager.h"

#include <memory>

#include "WorldIrrlicht.h"
#include "Application.h"

using namespace irr;
using namespace scene;
using namespace video;

using namespace world;

//----- ObjectNodeHandler

ObjectNodeHandler::ObjectNodeHandler(ObjectsManager &objectsManager, const Object3D & object)
	: _objManager(objectsManager), _meshNode(NULL) {

	updateObject3D(object);
}

ObjectNodeHandler::~ObjectNodeHandler() {
	_meshNode->remove();
}

void ObjectNodeHandler::updateObject3D(const Object3D & object) {
	SMesh * irrMesh = ObjectsManager::convertToIrrlichtMesh(object.getMesh(), _objManager._driver);

	if (_meshNode == NULL) {
		_meshNode = _objManager._sceneManager->addMeshSceneNode(irrMesh, 0, -1);
	}
	else {
		_meshNode->setMesh(irrMesh);
	}

	_meshNode->setPosition(toIrrlicht(object.getPosition()));
    //std::cout << "graph : " << object.getPosition() << std::endl;
	_meshNode->setScale(toIrrlicht(object.getScale()));

	// Materiau
	_meshNode->setMaterialFlag(EMF_LIGHTING, true);
	
	SMaterial & material = _meshNode->getMaterial(0);
	material.NormalizeNormals = true;
	material.BackfaceCulling = false;
	material.ColorMaterial = ECM_NONE;

	material.AmbientColor.set(255, 0, 0, 0);
	material.SpecularColor.set(255, 255, 255, 255);
	material.DiffuseColor.set(255, 100, 50, 0);

	irrMesh->drop();

    //_objManager._sceneManager->getActiveCamera()->setTarget(_meshNode->getPosition());
}


//----- ObjectsManager

ObjectsManager::ObjectsManager(Application & app, IrrlichtDevice * device)
	: RenderingModule(app, device) {

}

ObjectsManager::~ObjectsManager() {

}

void ObjectsManager::initialize(FlatWorldCollector &collector) {
	_objects.clear();

	update(collector);
}

void ObjectsManager::update(FlatWorldCollector &collector) {
	// Set remove tag to true
	for (auto & pair : _objects) {
		pair.second->removeTag = true;
	}

	// Add new objects
	auto it = collector.iterateItems();

	for (; it.hasNext(); ++it) {
		auto pair = *it;

		if (_objects.find(pair.first) == _objects.end()) {
			_objects[pair.first] =
					std::make_unique<ObjectNodeHandler>(*this, pair.second->getObject3D());
		}
		else {
			_objects[pair.first]->removeTag = false;
		}
	}

	// Remove all objects that have the remove tag
	auto iter = _objects.begin();

	for (; iter != _objects.end();) {
		if (iter->second->removeTag) {
			iter = _objects.erase(iter);
		}
		else {
			++iter;
		}
	}
}



SMesh * ObjectsManager::convertToIrrlichtMesh(const Mesh & mesh, IVideoDriver * driver) {
	SMesh * irrMesh = new SMesh();
	irr::s32 maxPrimitives = driver->getMaximalPrimitiveCount();
	auto & vertices = mesh.getVertices();

	auto & faces = mesh.getFaces();
	int primitiveCount = 0;

	int bufID = -1;
	SMeshBuffer * buffer;

	for (const Face & face : faces) {
		if (face.vertexCount() != 3)
			continue;

		// Switch to next buffer if the current one is filled
		if (bufID == -1 || primitiveCount + 3 >= maxPrimitives) {
			// last calculations on current buffer
			if (bufID != -1) {
				buffer->recalculateBoundingBox();
			}

			// switch to new buffer
			bufID++;

			if (bufID < irrMesh->getMeshBufferCount()) {
				buffer = (SMeshBuffer*)irrMesh->getMeshBuffer(bufID);
			}
			else {
				buffer = new SMeshBuffer();
				irrMesh->addMeshBuffer(buffer);
				buffer->drop();
			}

			buffer->Vertices.set_used(faces.size() * 3);
			buffer->Indices.set_used(faces.size() * 3);
		}

		// Add face data
		for (int i = 0; i < 3; i++) {
			int id = face.getID(i);

			S3DVertex& v = buffer->Vertices[primitiveCount];
			v.Pos = toIrrlicht(vertices.at(id).getPosition());
			v.TCoords = toIrrlicht(vertices.at(id).getTexture());
			v.Normal = toIrrlicht(vertices.at(id).getNormal());

			v.Color = SColor(255, 255, 255, 255);

			buffer->Indices[primitiveCount] = primitiveCount;

			primitiveCount++;
		}
	}

	irrMesh->setDirty();
	irrMesh->recalculateBoundingBox();

	return irrMesh;
}
