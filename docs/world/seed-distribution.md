# Plant species distribution using seeds

One of the most exciting things to do when creating a world, is to add details to it.
One way to add details to the world is vegetation generation, for example grass or trees generation.

World embeds some modules to create grass blades, bushes, trees of different types, etc. But what we
need then, is a way to populate our virtual world with all these vegetation models.

## First approach

The simplest thing we can do is to distribute the models randomly. For example, we can sample
`N` random positions in a chunk, `N` being determined according to a density we've set beforehand.
This method give good results for plains, but lack some realism for other landscapes such as the
top of a mountain or the ocean floor. We don't want trees underwater!

First thing we can do to address this issue, is to adapt the density of vegetation to the altitude.
Vegetation density will drop gradually as we get higher, and be set to 0 underwater.

After doing that, density is no more uniform inside of a chunk. But if we keep sampling points randomly in the
chunk, the final positions will still have an uniform repartition throught the chunk. We need to change
the algorithm to take this into account.

The idea here is to compute the maximum density of the chunk, then pick `N` according to this density.
For each sampled point, we can decide to drop it with probability `1 - real density / maximum density`.

<!-- Illustration -->

Now we have a good way to distribute the vegetation accross the world. As improvement we can also take slope
into account, because trees are growing more easily on gentle slope.

<!-- Also consider other distributions methods than pure random (like sampling series used in render engines...) -->

## Introducing species

Having vegetation on the world is great, but what we want to add is more diversity. With World we
can create several tree generators that have different parameters. The generators will then produce instances
that are very similar to each others: we have different species of trees! Each generator embeds a way to
randomize its parameters to create new species on the fly.