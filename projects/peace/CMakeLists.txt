
if (${WORLD_BUILD_PEACE})
    message("-- Build Peace Shared Library")

    add_library(peace SHARED
            csrc/world.cpp
            csrc/collector.cpp
            csrc/assets.cpp
            csrc/util.cpp
            csrc/terrains.cpp)

    target_link_libraries(peace world)

    # TODO Use a list of libraries instead of checking for all projects ?
    if (${WORLD_BUILD_VULKAN_MODULES} AND ${WORLD_HAS_VULKAN})
        message(STATUS "Build Peace with VkWorld support")
        target_compile_definitions(peace PRIVATE USE_VKWORLD=1)
        target_link_libraries(peace vkworld)
    endif()
	
    # TODO Disable dependency copying when building in STATIC mode
	if(WIN32)
		set(PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Peace/Plugins/Windows)
	else()
		set(PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Peace/Plugins/Linux)
	endif()

    add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:world> ${PLUGINS_DIR})
    add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:peace> ${PLUGINS_DIR})

    if (${WORLD_BUILD_VULKAN_MODULES} AND ${WORLD_HAS_VULKAN})
        add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vkworld> ${PLUGINS_DIR})
    endif()

	# ==== AUTOMATIC BUILDS WITH UNITY

	# Find unity executable
	set(UNITY_VERSION "2019.4.21f1")
    find_file(UNITY_EXE
        NAMES
            Unity.exe
            Unity
        PATHS
            $ENV{HOME}/.unity/
        PATH_SUFFIXES
            ${UNITY_VERSION}/Editor/
    )

	if(UNITY_EXE)
		message(STATUS "Found unity executable at ${UNITY_EXE}")

		# Add target for automatic plugin generation
		add_custom_target(peace_package 
			COMMAND ${UNITY_EXE} -batchmode -nographics -projectPath "${CMAKE_CURRENT_SOURCE_DIR}" -exportPackage "Assets/Peace" "${WORLD_BINARY_OUTPUT_DIRECTORY}/peace_${WORLD_VERSION}.unitypackage" -quit
			DEPENDS peace)

		# Add target for automatic demo build
		if (WIN32)
			add_custom_target(peace_demo
				COMMAND ${UNITY_EXE} -batchmode -nographics -projectPath "${CMAKE_CURRENT_SOURCE_DIR}" -buildWindows64Player "${WORLD_BINARY_OUTPUT_DIRECTORY}/WorldDemo/WorldDemo.exe" -quit
				DEPENDS peace)
		else()
			add_custom_target(peace_demo
				COMMAND ${UNITY_EXE} -batchmode -nographics -projectPath "${CMAKE_CURRENT_SOURCE_DIR}" -buildLinux64Player "${WORLD_BINARY_OUTPUT_DIRECTORY}/WorldDemo/WorldDemo" -quit
				DEPENDS peace)
		endif()

		if (${WORLD_BUILD_VULKAN_MODULES} AND ${WORLD_HAS_VULKAN})
			add_custom_command(TARGET peace_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/examples/worlds/vulkan-demo.json" "${WORLD_BINARY_OUTPUT_DIRECTORY}/WorldDemo/world.json")
		else()
			add_custom_command(TARGET peace_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/examples/worlds/demo.json" "${WORLD_BINARY_OUTPUT_DIRECTORY}/WorldDemo/world.json")
		endif()

		set_target_properties(peace_package peace_demo
			PROPERTIES EXCLUDE_FROM_ALL ON)
	else()
		message(STATUS "Unity executable (version = ${UNITY_VERSION}) was not found")
	endif()
endif()
