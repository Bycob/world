#include <iostream>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>

#include <world/core.h>

#include <vkworld/Vulkan.h>
#include <vkworld/ProgramVk.h>
#include <vkworld/BufferVk.h>
#include <vkworld/ProxyGround.h>

using namespace world;

void testVulkanVersion(int argc, char **argv);
void testProxyGround(int argc, char **argv);

int main(int argc, char** argv) {
	// testVulkanVersion(argc, argv);
	testProxyGround(argc, argv);
}

void testVulkanVersion(int argc, char ** argv) {
	auto &context = Vulkan::context();

	ProgramVk program(context);
	program.setEmbeddedShader("noise-perlin");

	// Initialize p
	u32 p[512];

	for (u32 i = 0; i < 256; ++i) {
		p[i] = i;
	}

	std::mt19937 rand(123156456);
	std::shuffle(std::begin(p), std::begin(p) + 256, rand);

	for (u32 i = 0; i < 256; ++i) {
		p[i + 256] = p[i];
	}

	// Create p buffer
	BufferVk pbuf(context, DescriptorType::STORAGE_BUFFER, sizeof(p));
	pbuf.setData(p, sizeof(p));
	
	// Create output data uniform
	const u32 img_width = 2048;
	const u32 img_height = 2048;

	struct {
		u32 width = img_width;
		u32 height = img_height;
		u32 depth = 1;
	} s_outputData;
	BufferVk outputData(context, DescriptorType::UNIFORM_BUFFER, sizeof(s_outputData));
	outputData.setData(&s_outputData, sizeof(s_outputData));

	// Create perlin data uniform
	struct {
		u32 octaves = 12;
		u32 octaveRef = 0;
		s32 offsetX = 1;
		s32 offsetY = 0;
		s32 offsetZ = 0;
		float frequence = 8;
		float persistence = 0.4;
	} s_perlinData;
	BufferVk perlinData(context, DescriptorType::UNIFORM_BUFFER, sizeof(s_perlinData));
	perlinData.setData(&s_perlinData, sizeof(s_perlinData));

	// Create output buffer
	u32 outputSize = img_width * img_height;
	BufferVk output(context, DescriptorType::STORAGE_BUFFER, outputSize * sizeof(float));

	// set uniforms
	program.setUniform(0, outputData);
	program.setUniform(1, perlinData);

	program.setBuffer(2, pbuf);
	program.setBuffer(3, output);

	program.setDispatch(img_width / 32, img_height / 32, 1);

	program.run();

	// Create an image embedding the result
	float *result = new float[outputSize];
	output.getData(result, outputSize * sizeof(float));

	Image img(img_width, img_height, ImageType::GREYSCALE);

	for (u32 y = 0; y < img_height; ++y) {
		for (u32 x = 0; x < img_width; ++x) {
			float r = result[y * img_width + x];
			img.grey(x, y).setLevelf(r);
		}
	}

	createDirectories("assets/vulkan/");
	img.write("assets/vulkan/perlinsh.png");

	delete[] result;
}

void testProxyGround(int argc, char **argv) {
	auto &context = Vulkan::context();

	// Create a proxyground 16 meters large, with at least one point per centimeter
	ProxyGround proxyGround(16, 1600);

	Collector collector;
	auto &imgChan = collector.addStorageChannel<Image>();

	proxyGround.collectAll(collector, 100);

	world::createDirectories("assets/vulkan/proxyground/");
	for (auto &entry : imgChan) {
		std::string id =ItemKeys::toString(entry._key);
		std::transform(id.begin(), id.end(), id.begin(), [](char a) {
			return a == '/' ? '_' : a;
		});
		id = std::string("assets/vulkan/proxyground/") + id + ".png";
		// std::cout << id << std::endl;
		entry._value.write(id);
	}
}
