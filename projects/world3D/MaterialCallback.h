#ifndef WORLD_MATERIALCALLBACK_H
#define WORLD_MATERIALCALLBACK_H

#include "WorldIrrlicht.h"

#include "ObjectsManager.h"

using namespace irr;

class MaterialCallback : public irr::video::IShaderConstantSetCallBack {
public:
    MaterialCallback(const world::Material &material) : _material{material} {}

    std::set<std::string> getTextures() {
        std::set<std::string> textures;

        for (const auto &keyval : _material.getShaderParams()) {
            const auto &key = keyval.first;
            const auto &param = keyval.second;
            using SType = world::ShaderParam::Type;

            if (param._type == SType::TEXTURE_ARRAY) {
                auto values = world::split(param._value, ',');
                textures.insert(values.begin(), values.end());
            } else if (param._type == SType::TEXTURE) {
                textures.insert(param._value);
            }
        }

        return textures;
    }

    void setTextureID(std::string texture, int id) {
        _textures[std::move(texture)] = id;
    }

    void OnSetConstants(video::IMaterialRendererServices *services,
                        s32 userData) override {
        auto *driver = services->getVideoDriver();

        // World View Proj matrixes for irrlicht
        core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
        invWorld.makeInverse();
        services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

        core::matrix4 worldViewProj;
        worldViewProj = driver->getTransform(video::ETS_PROJECTION);
        worldViewProj *= driver->getTransform(video::ETS_VIEW);
        worldViewProj *= driver->getTransform(video::ETS_WORLD);
        services->setVertexShaderConstant("mWorldViewProj",
                                          worldViewProj.pointer(), 16);

        core::matrix4 world = driver->getTransform(video::ETS_WORLD);
        world = world.getTransposed();
        services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

        // Parameters from world
        for (const auto &keyval : _material.getShaderParams()) {
            const auto &key = keyval.first;
            const auto &param = keyval.second;
            using SType = world::ShaderParam::Type;

            std::vector<s32> texIDs;
            std::vector<std::string> textures;

            switch (param._type) {
            case SType::TEXTURE_ARRAY:
                textures = world::split(param._value, ',');
            case SType::TEXTURE:
                textures.push_back(param._value);
                for (auto &texture : textures) {
                    texIDs.push_back(_textures.at(texture));
                }
                services->setVertexShaderConstant(key.c_str(), &texIDs[0],
                                                  texIDs.size());
                break;
            default:
                break; // If type is unknown, parameter is ignored
            }
        }
    }

private:
    world::Material _material;
    /// Texture to id in material
    std::map<std::string, int> _textures;
};

#endif // WORLD_MATERIALCALLBACK_H
