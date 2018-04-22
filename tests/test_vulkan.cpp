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

	// Code example
	/*Program program;
	program.addEmbeddedShader("shader-name");

	Buffer input1(1024);
	Buffer input2 = otherVkBuffer;

	// Fill the buffers
	buffer1.setData(matrix.data());

	program.setInputBuffer(0, input1);
	program.setInputBuffer(1, input2);

	Buffer output(1024 * 1024 * 3);
	
	program.setOutputBuffer(0, output);
	program.run();

	Image result(buffer);
	*/
}
