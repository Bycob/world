# What is World?

## An open, procedurally generated world

The aim of the *World* project is to create a complete and complex 3D environment
only using procedural generation techniques. Everything is procedural: geometry, materials,
textures... No assets, only code!

As we want our world to be huge, but also finely detailed, World implements LOD
support. You can go as far as you want, but also as close as you want.

Procedural generation enables you to tweak the parameters as you want. As World relies
solely on procedural generation, everything is configurable. You can therefore build a
world that fits your needs, or explore the vast amount of possibilities by changing
the parameters.

*World* is built as a library, so it is easy to integrate in other programs, and it
can perform offline assets generation that can be edited on 3D software, like blender.

Currently *World* can generate terrain, including mountains, plains and oceans, but
also trees, grass, rocks...

### About the "full procedural" policy

For good quality 3D environment, procedural techniques should be used in combination
with assets made by artists.

*World* is a challenge, it does not aim to be as good as what an artist could produce.
But procedural generation brings an other great feature: diversity. This library will
eventually (hopefully) be able to generate worlds that you can explore endlessly.
With species and biome generation on the fly, we can achieve continuous surprises and
new unseen landscapes.

## How does it work?

*World* needs to virtually generate an infinite amount of objects in space, but also
infinite level of details. Of course we can not store an infinite amount of objects,
nor generate it in finite time. That's why World generates only what is needed.

The structure of the world is a graph where the nodes are generators of a little part of
the world. The nodes are not 3D objects or meshes, but rather contain information about
how to generate the geometry and the texture at different level of details. What is
generated then depends on the position of the observer when they explore the world.

When a world is initialized, it contains no or very few nodes. New nodes are generated
during exploration by an entity called **Decorator**, that will populate the regions when
they are discovered for the first time. Some nodes also generate children nodes, to
add an additional level of detail for example.

### Exploration

**Resolution Models** are describing how a user see the world by associating a
resolution to every point in space. For example, a "first person resolution model",
would associate high resolution to a point near the player, and a very low resolution
to a point far away from the player.

To make the library generate assets for a part of the world, you have to explore it.
The resolution model will be used to determine which regions need exploration.
The result of the exploration (ie. the generated assets) will be stored in a
**Collector**. The collector has multiple channels that can store different types
of assets, going from meshes to materials and even shaders. Usually the collector
also retrieve "scene objects" that hold information about how the scene organization:
how the objects are positioned, what materials do they use, etc.

Once the collector is filled with all the generated assets, you can just iterate
through the list of all the scene objects, add them to a game engine scene for
rendering and voilà!

<!-- TODO Include a note about memory usage -->

## A powerful library

Due to its generic structure, *World* can have a large variety of uses.
You can use it to generate a complete game environment in real time, but also
only parts of the environment. You can even use *World* as a part of your workflow,
by generating the environment and then refining it manually. Or you can chose to
only use one module to generate trees or rocks for example.

*World* is a cross-platform library, available on Linux and Windows. The idea is
to make it compatible with as much tools and languages as possible. At the moment
you can use it in your C++ project and also as a (still limited) Unity plugin. You
can also export the generated assets into an .obj file to open it in your favorite
3D software.

In the near future a JSON API will be developped to ease the interoperability between
the core library and any application. Python bindings are also planned, as well as
other export formats for the 3D scene.
