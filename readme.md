World - ENGLISH
==========

Setup development environment
==========

Dependencies :
----------

**PYTHON**

Required : 
- numpy
- PIL

**C++**

Required :
- openCV

WorldGUI :
- QT

WorldExplorer :
- (irrlicht) (not yet implemented)

Build
-------

**WorldAPI**

In a terminal, get into your personnal *build* directory, then type

```
cmake [path_to_source] -G[your_favorite_compiler] -DCMAKE_PREFIX_PATH=[path to openCV]
```

then build and compile with *your_favorite_compiler*

**WorldGUI**

You can use Qt creator to build this target easily. Just open the *.pro* file, then configure your kit.
Just add this to the qmake command line : `"WORLDAPI_SOURCES=[path_to_source]" "WORLDAPI_LIBDIR=[location_of_built_worldapi]"`

Then ensure you have the .dll required to run the application in the qt build folder.



World - FRANCAIS
===========

World est un projet ambitieux, visant à fournir des outils nécessaires pour générer procéduralement des éléments
de la réalité, et également de les assembler pour pouvoir créer un écosystème complet.

