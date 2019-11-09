#include <iostream>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>

#include <world/core.h>
#include <world/terrain.h>

#include <vkworld/wrappers/Vulkan.h>
#include <vkworld/wrappers/VkwRandomTexture.h>
#include <vkworld/ProxyGround.h>
#include <vkworld/MultilayerGroundTexture.h>
#include <vkworld/VkwTextureGenerator.h>
#include <vkworld/VkwGrass.h>

using namespace world;

void testProxyGround(int argc, char **argv);
void testMultilayerTerrainTexture(int argc, char **argv);
void testTextureGenerator();
void testVkwGrass();

int main(int argc, char **argv) {
    // testProxyGround(argc, argv);
    // testMultilayerTerrainTexture(argc, argv);
    // testTextureGenerator();
    testVkwGrass();
}

void testProxyGround(int argc, char **argv) {
    auto &context = Vulkan::context();

    // Create a proxyground 16 meters large, with at least one point per
    // centimeter
    ProxyGround proxyGround(16, 1600);

    Collector collector;
    auto &imgChan = collector.addStorageChannel<Image>();

    proxyGround.collectAll(collector, 100);

    world::createDirectories("assets/vulkan/proxyground/");
    for (const auto &entry : imgChan) {
        std::string id = entry._key.str();
        id = std::string("assets/vulkan/proxyground/") + id + ".png";
        // std::cout << id << std::endl;
        entry._value.write(id);
    }
}

void testMultilayerTerrainTexture(int argc, char **argv) {
    Terrain terrain(128);
    terrain.setBounds(-500, -500, 0, 500, 500, 400);
    terrain.setTexture(Image(128 * 16, 128 * 16, ImageType::RGBA));

    PerlinTerrainGenerator terrainGen(5, 4, 0.4);
    terrainGen.processTerrain(terrain);

    MultilayerGroundTexture textureGen;
    textureGen.addDefaultLayers();
    textureGen.processTerrain(terrain);
    terrain.setBounds(-0.5, -0.5, 0, 0.5, 0.5, 0.4);

    Mesh *mesh = terrain.createMesh();
    SceneNode object("mesh1");
    object.setMaterialID("multilayer");

    Material mat("multilayer");
    mat.setMapKd("multilayer_texture.png");

    Scene scene;
    scene.addNode(object);
    scene.addMesh("mesh1", *mesh);
    scene.addMaterial("multilayer", mat);
    scene.addTexture("multilayer_texture.png", terrain.getTexture());


    world::createDirectories("assets/vulkan/multilayer/");
    ObjLoader obj;
    obj.write(scene, "assets/vulkan/multilayer/multilayer.obj");

    delete mesh;
}

void testTextureGenerator() {
#define GEN_SHADER_NAME "grass.frag"
#define GEN_RAND_TEX

    const u32 size = 1024;
    VkwTextureGenerator generator(size, size, GEN_SHADER_NAME);

#ifdef GEN_GRASS
    // Texture size
    struct {
        float offsetX = 0, offsetY = 0;
        float sizeX = 0.5, sizeY = 0.5;
    } textureStruct;
    generator.addParameter(0, DescriptorType::UNIFORM_BUFFER,
                           MemoryUsage::CPU_WRITES, sizeof(textureStruct),
                           &textureStruct);
#endif

#ifdef GEN_PERLIN
    // Random
    std::mt19937 _rng(42);
    std::vector<u32> random(256);
    std::iota(random.begin(), random.end(), 0);
    std::shuffle(random.begin(), random.end(), _rng);
    random.insert(random.end(), random.begin(), random.end());

    generator.addParameter(256, DescriptorType::STORAGE_BUFFER,
                           MemoryUsage::CPU_WRITES, random.size() * sizeof(u32),
                           &random[0]);
#endif

#ifdef GEN_RAND_TEX
    // Random texture
    VkwRandomTexture randomTex;
    generator.addImageParameter(0, randomTex.get());
#endif

    generator.generateTexture().write("assets/vulkan/test_generator.png");
}

void testVkwGrass() {
    VkwGrass grass;

    Collector collector(CollectorPresets::SCENE);
    grass.collectAll(collector, 10);

    Image img = (*collector.getStorageChannel<Image>().begin())._value;
    img.write("assets/vulkan/vkwgrass.png");
}
