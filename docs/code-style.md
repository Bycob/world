# The World coding style guide

In this document you can find all the coding conventions used in
this project.

## Many different sections...

    TODO

## Using third party libraries

### Armadillo

World uses armadillo to perform operations over 2-dimensional and
3-dimensional arrays.

When these arrays are used in a Cartesian space with dimensions XYZ,
the row number corresponds to the x coordinate, the column number
corresponds to the y coordinate, and the slice number corresponds to
the z coordinate. (This is different from images, where x corresponds
to the column.)

Thus, elements are accessed like this :

```cpp
    mat buffer1(25, 25);
    element_at_xy = buffer1(x, y);

    cube buffer2(25, 25, 25);
    element_at_xyz = buffer2(x, y, z);
```

Armadillo matrix are stored in column-major ordering. So the fastest way
to browse a mat or a cube are :

```cpp
    mat buffer1(25, 25);

    for (auto y = 0; y < buffer1.n_cols; ++y) {
        for (auto x = 0; x < buffer1.n_rows; ++x) {
            element_at_xy = buffer1(x, y);
        }
    }

    cube buffer2(25, 25, 25);
    // slices are contiguous in memory
    for (auto z = 0; z < buffer2.n_slices; ++z) {
        for (auto y = 0; y < buffer2.n_cols; ++y) {
            for (auto x = 0; x < buffer2.n_rows; ++x) {
                element_at_xyz = buffer2(x, y, z);
            }
        }
    }
```
