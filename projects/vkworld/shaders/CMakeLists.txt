message(STATUS "Build shaders with ${Vulkan_GLSL_VALIDATOR}")

file(
	GLOB
	WORLD_SHADERS_SOURCES
	"./*.comp" "./*.frag" "./*.vert"
)

file(
	GLOB
	WORLD_SHADERLIB_SOURCES
	lib/*.comp
)

set(WORLD_SHADERS_BIN ${WORLD_SHADERLIB_SOURCES})

# Choose output directory according to the target and the compiler
if(WORLD_COMPILER_IS_MSVC)
	set(WORLD_SHADERS_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
else()
	set(WORLD_SHADERS_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
set(WORLD_SHADERS_OUTPUT_DIRECTORY ${WORLD_SHADERS_OUTPUT_DIRECTORY}/shaders)

message(STATUS "Shader output directory is: ${WORLD_SHADERS_OUTPUT_DIRECTORY}")

# Compile all shaders
foreach(SOURCE_SHADER ${WORLD_SHADERS_SOURCES})
	get_filename_component(SHADER_NAME "${SOURCE_SHADER}" NAME)
	set(COMPILED_SHADER "${WORLD_SHADERS_OUTPUT_DIRECTORY}/${SHADER_NAME}.spv")

	add_custom_command(
		OUTPUT ${COMPILED_SHADER}
		COMMAND ${Vulkan_GLSL_VALIDATOR} 
		ARGS -V ${SOURCE_SHADER} -o ${COMPILED_SHADER}
		MAIN_DEPENDENCY ${SOURCE_SHADER}
		COMMENT "Compiling shader ${SHADER_NAME}"
		VERBATIM
	)

	set(WORLD_SHADERS_BIN ${WORLD_SHADERS_BIN} ${COMPILED_SHADER})
endforeach()

# Add shaders to resource library
rclib_add_resource_library(shaderlib world ${WORLD_SHADERS_BIN})
