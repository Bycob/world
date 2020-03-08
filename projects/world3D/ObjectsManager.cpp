#include "ObjectsManager.h"

#include <memory>
#include <world/core.h>

#include "WorldIrrlicht.h"
#include "Application.h"
#include "MaterialCallback.h"

using namespace irr;
using namespace scene;
using namespace video;

using namespace world;

//----- ObjectNodeHandler

ObjectNodeHandler::ObjectNodeHandler(ObjectsManager &objectsManager,
                                     const SceneNode &object,
                                     Collector &collector)
        : _objManager(objectsManager), _meshNode(NULL) {

    updateObject3D(object, collector);

    // Material
    auto &matChan = collector.getStorageChannel<Material>();

    if (matChan.has(key(object.getMaterialID()))) {
        auto &material = matChan.get(key(object.getMaterialID()));
        setMaterial(material, collector);
    }
}

ObjectNodeHandler::~ObjectNodeHandler() {
    _meshNode->remove();

    for (std::string &texId : _usedTextures) {
        _objManager.removeTextureUser(texId);
    }
}

bool ObjectNodeHandler::setTexture(int id, const std::string &path,
                                   Collector &collector) {
    SMaterial &irrmat = _meshNode->getMaterial(0);
    auto texID = _objManager.getOrLoadTexture(path, collector);

    if (texID != nullptr) {
        irrmat.setTexture(id, texID);
        _objManager.addTextureUser(path);
        _usedTextures.emplace_back(path);
        return true;
    }
    return false;
}

void ObjectNodeHandler::setMaterial(const Material &mat, Collector &collector) {
    SMaterial &irrmat = _meshNode->getMaterial(0);

    irrmat.AmbientColor = toIrrColor(mat.getKa());
    irrmat.SpecularColor = toIrrColor(mat.getKs());
    irrmat.DiffuseColor = toIrrColor(mat.getKd());

    irrmat.TextureLayer[0].BilinearFilter = true;
    irrmat.TextureLayer[0].TrilinearFilter = true;

    if (mat.isTransparent()) {
        irrmat.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
    }

    // Textures
    int texstart = 0;
    if (setTexture(0, mat.getMapKd(), collector))
        texstart = 1;

    // Shader
    std::string shaderName = mat.getShader();
    auto it = _objManager._loadedShaders.find(shaderName);
    if (it != _objManager._loadedShaders.end()) {
        irrmat.MaterialType = it->second;
    }

    if (!shaderName.empty() && collector.hasStorageChannel<Shader>()) {
        auto &shaderChan = collector.getStorageChannel<Shader>();

        if (shaderChan.has(key(shaderName))) {
            video::IGPUProgrammingServices *gpu =
                _objManager._driver->getGPUProgrammingServices();
            auto *mc = new MaterialCallback(mat);

            for (auto &texture : mc->getTextures()) {
                if (setTexture(texstart, texture, collector)) {
                    mc->setTextureID(texture, texstart);
                    texstart++;
                } else {
                    // callback requires an id for every texture
                    mc->setTextureID(texture, 0);
                }
            }

            const Shader &shader = shaderChan.get(shaderName);
            s32 shaderMat = gpu->addHighLevelShaderMaterialFromFiles(
                shader.getVertexPath().c_str(), "main", video::EVST_VS_1_1,
                shader.getFragmentPath().c_str(), "main", video::EPST_PS_1_1,
                mc, video::EMT_SOLID, 0, video::EGSL_DEFAULT);

            irrmat.MaterialType =
                static_cast<video::E_MATERIAL_TYPE>(shaderMat);
            _objManager._loadedShaders[shaderName] = irrmat.MaterialType;

            mc->drop();
        }
    }

    for (u32 i = 1; i < _meshNode->getMaterialCount(); ++i) {
        _meshNode->getMaterial(i) = irrmat;
    }
}

void ObjectNodeHandler::updateObject3D(const SceneNode &object,
                                       Collector &collector) {
    auto &meshChannel = collector.getStorageChannel<Mesh>();

    if (!meshChannel.has(key(object.getMeshID()))) {
        return;
    }
    const Mesh &mesh = meshChannel.get(key(object.getMeshID()));
    SMesh *irrMesh =
        ObjectsManager::convertToIrrlichtMesh(mesh, _objManager._driver);

    if (_meshNode == NULL) {
        _meshNode = _objManager._sceneManager->addMeshSceneNode(irrMesh, 0, -1);
    } else {
        _meshNode->setMesh(irrMesh);
    }

    _meshNode->setPosition(toIrrlicht(object.getPosition()));
    // std::cout << "graph : " << object.getPosition() << std::endl;
    _meshNode->setScale(toIrrlicht(object.getScale()));

    // Material
    _meshNode->setMaterialFlag(EMF_LIGHTING, true);
    _meshNode->setMaterialFlag(EMF_FOG_ENABLE, true);

    SMaterial &material = _meshNode->getMaterial(0);
    material.BackfaceCulling = false;
    material.ColorMaterial = ECM_NONE;

    material.AmbientColor.set(255, 0, 0, 0);
    material.SpecularColor.set(255, 255, 255, 255);
    material.DiffuseColor.set(255, 200, 178, 126); // 100, 50, 0);

    for (u32 i = 1; i < _meshNode->getMaterialCount(); ++i) {
        _meshNode->getMaterial(i) = material;
    }
    irrMesh->drop();

    //_objManager._sceneManager->getActiveCamera()->setTarget(_meshNode->getPosition());
}


//----- ObjectsManager

ObjectsManager::ObjectsManager(Application &app, IrrlichtDevice *device)
        : RenderingModule(app, device) {}

ObjectsManager::~ObjectsManager() {
    // To remove objects and thus textures.
    _objects.clear();
}

void ObjectsManager::initialize(Collector &collector) {
    _objects.clear();

    update(collector);
}

void ObjectsManager::update(Collector &collector) {
    auto start = std::chrono::steady_clock::now();

    if (!_partialUpdate) {
        _objects.clear();
    }

    // Set remove tag to true
    for (auto &pair : _objects) {
        pair.second->removeTag = true;
    }

    // Add new objects
    auto &objects = collector.getStorageChannel<SceneNode>();

    for (const auto &objectEntry : objects) {
        if (_objects.find(objectEntry._key) == _objects.end() ||
            !_partialUpdate) {
            auto objNode = std::make_unique<ObjectNodeHandler>(
                *this, objectEntry._value, collector);

            _objects[objectEntry._key] = std::move(objNode);

            _dbgAdded++;
        } else {
            _objects[objectEntry._key]->removeTag = false;
        }
    }

    // Remove all objects that have the remove tag
    auto iter = _objects.begin();

    for (; iter != _objects.end();) {
        if (iter->second->removeTag) {
            iter = _objects.erase(iter);
            _dbgRemoved++;
        } else {
            ++iter;
        }
    }
    _driver->removeAllHardwareBuffers();

    _elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();

    if (_dbgOn) {
        std::cout << _dbgAdded << " " << _dbgRemoved << " " << _elapsedTime
                  << std::endl;
    }
}

ITexture *ObjectsManager::getOrLoadTexture(const std::string &path,
                                           Collector &collector) {
    auto &texChan = collector.getStorageChannel<Image>();

    if (!texChan.has(key(path))) {
        return nullptr;
    }

    if (!_driver->findTexture(path.c_str())) {
        const Image &image = texChan.get(key(path));

        ImageStream stream(image);
        int dataSize = stream.remaining();
        char *data = new char[dataSize];
        stream.read(data, dataSize);

        IImage *irrimg = _driver->createImageFromData(
            image.type() == ImageType::RGBA ? ECF_A8R8G8B8
                                            : ECF_R8G8B8, // TODO support
            // greyscale
            {static_cast<irr::u32>(image.width()),
             static_cast<irr::u32>(image.height())},
            data, true);

        _driver->addTexture(path.c_str(), irrimg);
    }

    return _driver->getTexture(path.c_str());
}

void ObjectsManager::addTextureUser(const std::string &texId) {
    // TODO check if texId.c_str() is the id of texture for driver
    auto it = _loadedTextures.find(texId);

    if (it != _loadedTextures.end()) {
        ++it->second;
    } else {
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


SMesh *ObjectsManager::convertToIrrlichtMesh(const Mesh &mesh,
                                             IVideoDriver *driver) {
    auto *irrMesh = new SMesh();
    irr::s64 maxPrimitives = min(driver->getMaximalPrimitiveCount(), 0xFFFF);
    int primitiveCount = 0;

    int bufID = -1;
    SMeshBuffer *buffer = nullptr;

    for (world::u32 fid = 0; fid < mesh.getFaceCount(); fid++) {
        const Face &face = mesh.getFace(fid);

        if (face.vertexCount() != 3)
            continue;

        // Switch to next buffer if the current one is filled
        if (bufID == -1 || primitiveCount + 3 >= maxPrimitives) {
            // last calculations on current buffer
            if (bufID != -1) {
                buffer->Vertices.set_used(primitiveCount);
                buffer->Indices.set_used(primitiveCount);
                buffer->recalculateBoundingBox();
            }

            // switch to new buffer
            bufID++;

            if (bufID < irrMesh->getMeshBufferCount()) {
                buffer = (SMeshBuffer *)irrMesh->getMeshBuffer(bufID);
            } else {
                buffer = new SMeshBuffer();
                irrMesh->addMeshBuffer(buffer);
                buffer->drop();
            }

            buffer->Vertices.set_used(maxPrimitives);
            buffer->Indices.set_used(maxPrimitives);
            primitiveCount = 0;
        }

        // Add face data
        for (int i = 0; i < 3; i++) {
            int id = face.getID(i);

            S3DVertex &v = buffer->Vertices[primitiveCount];
            v.Pos = toIrrlicht(mesh.getVertex(id).getPosition());
            v.TCoords = toIrrlicht(
                mesh.getVertex(id).getTexture() * vec2d{1, -1} + vec2d{0, 1});
            v.Normal = toIrrlicht(mesh.getVertex(id).getNormal());

            v.Color = SColor(255, 255, 255, 255);

            buffer->Indices[primitiveCount] =
                static_cast<irr::u16>(primitiveCount);

            primitiveCount++;
        }
    }

    if (buffer != nullptr) {
        buffer->Vertices.set_used(primitiveCount);
        buffer->Indices.set_used(primitiveCount);
        buffer->recalculateBoundingBox();
    }

    irrMesh->setDirty();
    irrMesh->recalculateBoundingBox();

    return irrMesh;
}
