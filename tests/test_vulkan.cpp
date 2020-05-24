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
#include <vkworld/wrappers/VkwMemoryHelper.h>
#include <vkworld/VkwGrass.h>
#include <vkworld/VkwLeafTexture.h>

using namespace world;

void testCompute();
void testRandomImage();
void testMultilayerTerrainTexture(int argc, char **argv);
void testTextureGenerator();
void testVkwGrass();
void testLeaves();

int main(int argc, char **argv) {
    world::createDirectories("assets/vulkan/");

    // testCompute();
    // testMultilayerTerrainTexture(argc, argv);
    testTextureGenerator();
    // testVkwGrass();
    // testLeaves();
    // std::cout << "====" << std::endl;
    // testRandomImage();
}

void testCompute() {
    // This method contains several notes to improve the API later on
    int size = 1024;
    int groupSize = 32;
    int dispatchSize = size / groupSize;
    
    std::cout << "Set up..." << std::endl;

    VkwComputeWorker worker;

    VkwDescriptorSetLayout layout({0, 1}, {3, 256});

    // Buffers
    auto &ctx = Vulkan::context();

    // -- 0
    vec3u dimsVec(size, size, 1);
    auto dims = ctx.allocate(sizeof(vec3u), DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
    dims.setData(&dimsVec); // TODO add setData(const T &struct)

    // -- 1
    struct {
        u32 octaves = 5;
        u32 octaveRef = 0;
        s32 offsetX = 0;
        s32 offsetY = 0;
        s32 offsetZ = 0;
        float frequency = 4.0f;
        float persistence = 0.5f;
    } noiseStruct;

    auto noiseParams = ctx.allocate(sizeof(noiseStruct), DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
    noiseParams.setData(&noiseStruct);

    // -- 256
    Perlin perlin;
    std::vector<u32> randVec;

    for (u8 u : perlin.getHash()) {
        randVec.push_back(u);
    }

    auto random = ctx.allocate(randVec.size() * sizeof(u32), DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_WRITES);
    random.setData(&randVec[0]);

    // -- 3
    auto output = ctx.allocate(size * size * sizeof(float), DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_READS);


    VkwDescriptorSet dset(layout);
    dset.addDescriptor(0, dims);
    dset.addDescriptor(1, noiseParams);
    dset.addDescriptor(3, output);
    dset.addDescriptor(256, random);

    VkwComputePipeline pipeline(layout, "noise-perlin.comp");

    worker.bindCommand(pipeline, dset);
    worker.dispatchCommand(dispatchSize, dispatchSize, 1);
    worker.endCommandRecording(); // endCommand in run ?

    std::cout << "Running..." << std::endl;
    worker.run();
    worker.waitForCompletion();

    Image img(size, size, ImageType::GREYSCALE);
    VkwMemoryHelper::GPUToImage(output, img);

    std::cout << "Completed! Writing to assets/vulkan/compute.png" << std::endl;
    img.write("assets/vulkan/compute.png");
}

void testRandomImage() {
    // Write a random image, then read it from GPU and write it on disk
    VkwRandomTexture randTex(256);

    world::createDirectories("assets/vulkan/");
    VkwMemoryHelper::GPUToImage(randTex.get()).write("assets/vulkan/randTex.png");
    std::cout << "wrote assets/vulkan/randTex.png" << std::endl;
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
    std::cout << "wrote assets/vulkan/vkwgrass.png" << std::endl;
    img.write("assets/vulkan/vkwgrass.png");
}

void testLeaves() {
    VkwLeafTexture leaves;
    std::cout << "writing to "
              << "assets/vulkan/vkwleaf.png" << std::endl;
    leaves.generateLeafTexture().write("assets/vulkan/vkwleaf.png");
}
