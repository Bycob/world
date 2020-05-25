#include <iostream>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>

#include <world/core.h>
#include <world/terrain.h>

#include <vkworld/wrappers/Vulkan.h>
#include <vkworld/wrappers/VkwRandomTexture.h>
#include <vkworld/MultilayerGroundTextureOld.h>
#include <vkworld/VkwMultilayerGroundTexture.h>
#include <vkworld/wrappers/VkwTextureGenerator.h>
#include <vkworld/VkwGrass.h>
#include <vkworld/VkwLeafTexture.h>

using namespace world;

void testMultilayerTerrainTexture(int argc, char **argv);
void testTextureGenerator();
void testVkwGrass();
void testLeaves();

int main(int argc, char **argv) {
    // testMultilayerTerrainTexture(argc, argv);
    // testTextureGenerator();
    // testVkwGrass();
    testLeaves();
}

// TODO
// - Profiling
// - Clean world3D to use vkworld only if it is available
// - Change array of textures to texture arrays
// - Transition images correctly, use the right layout for the right usage
// - Fix vulkan warnings
// - Use the right compile definition with cmake. Documentation about it.
// - Implement what it takes to deallocate resources and not saturate the RAM
// - Address potential performance issues
void testMultilayerTerrainTexture(int argc, char **argv) {
    Terrain terrain(128);
    terrain.setBounds(-500, -500, 0, 500, 500, 400);
    terrain.setTexture(Image(128 * 16, 128 * 16, ImageType::RGBA));

    PerlinTerrainGenerator terrainGen(5, 4, 0.4);
    terrainGen.processTerrain(terrain);

    VkwMultilayerGroundTexture textureGen;
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

void testLeaves() {
    VkwLeafTexture leaves;
    std::cout << "writing to "
              << "assets/vulkan/vkwleaf.png" << std::endl;
    leaves.generateLeafTexture().write("assets/vulkan/vkwleaf.png");
}
