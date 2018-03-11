#include "ObjectsManager.h"

#include <memory>

#include "WorldIrrlicht.h"
#include "Application.h"

using namespace irr;
using namespace scene;
using namespace video;

using ObjectKey = WorldCollector::ObjectKey;

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
    std::cout << "graph : " << object.getPosition() << std::endl;
	_meshNode->setScale(toIrrlicht(object.getScale()));

	// Materiau
	_meshNode->setMaterialFlag(EMF_LIGHTING, true);
	
	SMaterial & material = _meshNode->getMaterial(0);
	material.NormalizeNormals = true;
	material.BackfaceCulling = false;
	material.ColorMaterial = ECM_NONE;

	material.AmbientColor.set(255, 0, 0, 0);
	material.SpecularColor.set(255, 255, 255, 255);
	material.DiffuseColor.set(255, 200, 178, 126);

	irrMesh->drop();
}


//----- ObjectsManager

ObjectsManager::ObjectsManager(Application & app, IrrlichtDevice * device)
	: RenderingModule(app, device) {

}

ObjectsManager::~ObjectsManager() {

}

void ObjectsManager::initialize(const FlatWorldCollector &collector) {
	_objects.clear();

	update(collector);
}

void ObjectsManager::update(const FlatWorldCollector &collector) {
	/*auto it = collector.objectsIterator();

	for (; it.hasNext(); it++) {
		auto pair = *it;

		if (!_objects.find(pair.first)) {
			auto & mainPart = pair.second.getPart(0);
			_objects[pair.first] =
					std::make_unique<ObjectNodeHandler>(*this, mainPart.getObject3D());
		}
	}*/
}



SMesh * ObjectsManager::convertToIrrlichtMesh(const Mesh & mesh, IVideoDriver * driver) {
	SMesh * irrMesh = new SMesh();
	s32 maxPrimitives = driver->getMaximalPrimitiveCount();

	auto & positions = mesh.getVertices<VType::POSITION>();
	auto & texCoords = mesh.getVertices<VType::TEXTURE>();
	auto & normals = mesh.getVertices<VType::NORMAL>();

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
			int posID = face.getID<VType::POSITION>(i);
			int texID = face.getID<VType::TEXTURE>(i);
			int normID = face.getID<VType::NORMAL>(i);

			S3DVertex& v = buffer->Vertices[primitiveCount];
			v.Pos = toIrrlicht(positions[posID].toVec3() * 100); // TODO check if posID == -1;
			
			if (texID != -1)
				v.TCoords = toIrrlicht(positions[texID].toVec2());

			if (normID != -1)
				v.Normal = toIrrlicht(positions[normID].toVec3());

			v.Color = SColor(255, 255, 255, 255);

			buffer->Indices[primitiveCount] = primitiveCount;

			primitiveCount++;
		}
	}

	irrMesh->setDirty();
	irrMesh->recalculateBoundingBox();

	return irrMesh;
}
