#include "ObjectsManager.h"

#include <memory>
#include <worldcore.h>

#include "WorldIrrlicht.h"
#include "Application.h"

using namespace irr;
using namespace scene;
using namespace video;

using namespace world;

//----- ObjectNodeHandler

ObjectNodeHandler::ObjectNodeHandler(ObjectsManager &objectsManager, const CollectorItem &provider)
	: _objManager(objectsManager), _meshNode(NULL) {

	const Object3D &object = provider.getObject3D();
	updateObject3D(object);

	// Material
	auto material = provider.getMaterial(object.getMaterialID());
	if (material) {
		setMaterial(*material, provider);
	}
}

ObjectNodeHandler::~ObjectNodeHandler() {
	_meshNode->remove();

	for (std::string &texId : _usedTextures) {
		_objManager.removeTextureUser(texId);
	}
}

void ObjectNodeHandler::setTexture(int id, const std::string &path, const world::CollectorItem &provider) {
	auto texture = provider.getTexture(path);

	if (!texture) {
		return;
	}

	auto driver = _objManager._driver;
	SMaterial & irrmat = _meshNode->getMaterial(0);

	std::string uid = texture->_uid;

	if (!driver->findTexture(uid.c_str())) {
		const Image &image = texture->_image;

		ImageStream stream(image);
		int dataSize = stream.remaining();
		char *data = new char[dataSize];
		stream.read(data, dataSize);

		IImage *irrimg = driver->createImageFromData(
				image.type() == ImageType::RGBA ? ECF_A8R8G8B8 : ECF_R8G8B8, // TODO support greyscale
				{static_cast<irr::u32>(image.width()), static_cast<irr::u32>(image.height())},
				data, true);

		driver->addTexture(uid.c_str(), irrimg);
	}

	irrmat.setTexture(id, driver->getTexture(uid.c_str()));
	_usedTextures.emplace_back(uid);
	_objManager.addTextureUser(uid);
}

void ObjectNodeHandler::setMaterial(const Material &mat, const CollectorItem &provider) {
	SMaterial & irrmat = _meshNode->getMaterial(0);

	irrmat.AmbientColor = toIrrColor(mat.getKa());
	irrmat.SpecularColor = toIrrColor(mat.getKs());
	irrmat.DiffuseColor = toIrrColor(mat.getKd());

	setTexture(0, mat.getMapKd(), provider);
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
	_meshNode->setMaterialFlag(EMF_FOG_ENABLE, true);
	
	SMaterial & material = _meshNode->getMaterial(0);
	material.BackfaceCulling = false;
	material.ColorMaterial = ECM_NONE;

	material.AmbientColor.set(255, 0, 0, 0);
	material.SpecularColor.set(255, 255, 255, 255);
	material.DiffuseColor.set(255, 200, 178, 126);//100, 50, 0);

	irrMesh->drop();

    //_objManager._sceneManager->getActiveCamera()->setTarget(_meshNode->getPosition());
}


//----- ObjectsManager

ObjectsManager::ObjectsManager(Application & app, IrrlichtDevice * device)
	: RenderingModule(app, device) {

}

ObjectsManager::~ObjectsManager() {
	// To remove objects and thus textures.
	_objects.clear();
}

void ObjectsManager::initialize(FlatWorldCollector &collector) {
	_objects.clear();

	update(collector);
}

void ObjectsManager::update(FlatWorldCollector &collector) {
	auto start = std::chrono::steady_clock::now();

	// Set remove tag to true
	for (auto & pair : _objects) {
		pair.second->removeTag = true;
	}

	// Add new objects
	auto it = collector.iterateItems();

	for (; it.hasNext(); ++it) {
		auto pair = *it;

		if (_objects.find(pair.first) == _objects.end()) {
			Object3D &object = pair.second->getObject3D();
			auto objNode = std::make_unique<ObjectNodeHandler>(*this, *pair.second);

			_objects[pair.first] = std::move(objNode);

			_dbgAdded++;
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
			_dbgRemoved++;
		}
		else {
			++iter;
		}
	}

	_elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

	if (_dbgOn) {
		std::cout << _dbgAdded << " " << _dbgRemoved << " " << _elapsedTime << std::endl;
	}
}

void ObjectsManager::addTextureUser(const std::string &texId) {
	auto it = _loadedTextures.find(texId);

	if (it != _loadedTextures.end()) {
		++it->second;
	}
	else {
		_loadedTextures.emplace(texId, 1);
	}
}

void ObjectsManager::removeTextureUser(const std::string &texId) {
	auto it = _loadedTextures.find(texId);

	if (it != _loadedTextures.end()) {
		--it->second;

		if (it->second == 0) {
			_loadedTextures.erase(it);
			_driver->removeTexture(_driver->getTexture(texId.c_str()));
		}
	}
}



SMesh * ObjectsManager::convertToIrrlichtMesh(const Mesh & mesh, IVideoDriver * driver) {
	SMesh * irrMesh = new SMesh();
	irr::s32 maxPrimitives = driver->getMaximalPrimitiveCount();
	int primitiveCount = 0;

	int bufID = -1;
	SMeshBuffer * buffer = nullptr;

	for (int fid = 0; fid < mesh.getFaceCount(); fid++) {
		const Face &face = mesh.getFace(fid);

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

			buffer->Vertices.set_used(mesh.getFaceCount() * 3);
			buffer->Indices.set_used(mesh.getFaceCount() * 3);
		}

		// Add face data
		for (int i = 0; i < 3; i++) {
			int id = face.getID(i);

			S3DVertex& v = buffer->Vertices[primitiveCount];
			v.Pos = toIrrlicht(mesh.getVertex(id).getPosition());
			v.TCoords = toIrrlicht(mesh.getVertex(id).getTexture() * vec2d{1, -1} + vec2d{0, 1});
			v.Normal = toIrrlicht(mesh.getVertex(id).getNormal());

			v.Color = SColor(255, 255, 255, 255);

			buffer->Indices[primitiveCount] = static_cast<irr::u16>(primitiveCount);

			primitiveCount++;
		}
	}

	if (buffer != nullptr) {
		buffer->recalculateBoundingBox();
	}

	irrMesh->setDirty();
	irrMesh->recalculateBoundingBox();

	return irrMesh;
}
