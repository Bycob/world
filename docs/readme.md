# World Documentation

Welcome to the general documentation about the World Library.
Here you can find general information about the library and
how to use it.

## Philosophy

*What does World actually do?*

With the World Library, it is possible to create a virtual
3D world and manipulate it. This world is unlimited,
in terms of wideness but also in terms of resolution. You
can go as far as you want, or look as close as you want, the
only limits are computational speed and available memory.

The World is manipulated through different *Actions*.

When a World is first created, it is empty. The "Decorate" actions
add new objects to the world, everywhere at once. In fact, objects
are not added immediately, because it is impossible as the world is
unlimited. Instead the action is registered, and then each time
a new portion of the world is explored, the action is applied to
this new portion.

## Structure

The world is structured as a tree of nodes, each node contains information
about particular content in a zone of the world.

