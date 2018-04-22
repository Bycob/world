if (WORLD_BUILD_VULKAN_MODULES AND WORLD_HAS_VULKAN)
	message("-- Build shaders with ${Vulkan_GLSL_VALIDATOR}")

	file(
		GLOB
		WORLD_SHADERS_SOURCES
		./*.comp
	)
	
	set(WORLD_SHADERS_BIN "")

	foreach(SOURCE_SHADER ${WORLD_SHADERS_SOURCES})
		get_filename_component(SHADER_NAME "${SOURCE_SHADER}" NAME_WE)
		set(COMPILED_SHADER "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${SHADER_NAME}.spv")

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

	add_custom_target(shaders DEPENDS ${WORLD_SHADERS_BIN})
	add_dependencies(worldapi shaders)
endif()
