# World - ENGLISH

# Setup development environment

## Dependencies :

### General

The World project is built using CMake. To install CMake you can either download it on
its [official website](https://cmake.org/install/), or, if you are a linux user,
just type in terminal : 
```
sudo apt-get install cmake
```

The C++ part of the project needs the following dependencies to build :
- openCV : you can download it [here](https://sourceforge.net/projects/opencvlibrary/)

Fortunately, openCV will be optionnal soon.

### Python

To use the python part of the project, you will need these two packages :
- numpy : `pip install --user numpy`
- Pillow : `pip install Pillow`

You can install pip with the command : `python -m pip install --upgrade pip`

### WorldGUI
- QT

### World3D
- irrlicht

All the other third parties are included in the repository.

## Build

**WorldAPI**

In a terminal, get into your personnal *build* directory, then type

```
cmake [path_to_source] -G[your_favorite_compiler] -DCMAKE_PREFIX_PATH=[path to openCV]
```

then build and compile with *your_favorite_compiler*

**WorldGUI**

You can use Qt creator to build this target easily. Just open the *.pro* file, then configure your kit.
Just add this to the qmake command line : `"WORLDAPI_SOURCES=[path_to_source]" "WORLDAPI_LIBDIR=[location_of_built_worldapi]"`

Then ensure you have the libraries required to run the application in the qt build folder.

# World - FRANCAIS

World est un projet visant à fournir des outils nécessaires pour générer procéduralement des éléments
de la réalité, et également de les assembler pour pouvoir créer un écosystème complet.

