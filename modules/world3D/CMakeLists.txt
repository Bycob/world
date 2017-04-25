
# Recupération des sources
set(WORLD3D_SOURCES
        Application.cpp
        Application.h
        IniFile.cpp
        IniFile.h
        main.cpp
        MainView.cpp
        MainView.h
        util.h)

# Ajout de la target
if(${WORLD_BUILD_WORLD3D})
    message("-- Build World3D")
    add_executable(world3D ${WORLD3D_SOURCES})
    target_link_libraries(world3D ${IRRLICHT_LIBRARIES} worldapi)
endif()
