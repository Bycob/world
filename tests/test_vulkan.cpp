#include <iostream>
#include <stdexcept>

#include <vulkan/Vulkan.h>

using namespace world;

void testVulkanVersion(int argc, char **argv);

int main(int argc, char** argv) {
	testVulkanVersion(argc, argv);
}

void testVulkanVersion(int argc, char ** argv) {
	Vulkan::context().configTest();
}
