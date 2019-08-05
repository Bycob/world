#include "Terrain.h"

#include <algorithm>
#include <iostream>
#include <math.h>

#include "world/assets/Interop.h"
#include "world/assets/Image.h"
#include "world/assets/Mesh.h"
#include "world/assets/MeshOps.h"
#include "world/math/MathsHelper.h"

using namespace arma;

namespace world {

Terrain::Terrain(int size)
        : _bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}), _array(size, size),
          _texture(1, 1, ImageType::RGB) {

    _texture.rgb(0, 0).set(255, 255, 255);
}

Terrain::Terrain(const Mat<double> &data)
        : _bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}), _array(data),
          _texture(1, 1, ImageType::RGB) {

    if (data.n_rows != data.n_cols) {
        throw std::runtime_error("Terrain must be squared !");
    }
    _texture.rgb(0, 0).set(255, 255, 255);
}

Terrain::Terrain(const Terrain &terrain)
        : _bbox(terrain._bbox), _array(terrain._array),
          _texture(terrain._texture) {}

Terrain::Terrain(Terrain &&terrain)
        : _bbox(terrain._bbox), _array(std::move(terrain._array)),
          _texture(std::move(terrain._texture)) {}

Terrain::~Terrain() = default;

Terrain &Terrain::operator=(const Terrain &terrain) {
    _bbox = terrain._bbox;
    _array = terrain._array;
    _texture = terrain._texture;
    return *this;
}

void Terrain::setBounds(double xmin, double ymin, double zmin, double xmax,
                        double ymax, double zmax) {
    _bbox.reset({xmin, ymin, zmin}, {xmax, ymax, zmax});
}

const BoundingBox &Terrain::getBoundingBox() const { return _bbox; }

vec3d Terrain::getNormal(int x, int y) const {
    int size_1 = getResolution() - 1;
    const double xUnit = 1. / size_1;
    const double yUnit = xUnit;

    int xa = max(x - 1, 0), xb = min(x + 1, size_1), xm = xb - xa;
    vec3d nx{(_array(xa, y) - _array(xb, y)), 0, xm * xUnit};

    int ya = max(y - 1, 0), yb = min(y + 1, size_1), ym = yb - ya;
    vec3d ny{0, (_array(x, ya) - _array(x, yb)), ym * yUnit};
    return (nx * ym + ny * xm).normalize();
}

double Terrain::getSlope(int x, int y) const {
    vec3d normal = getNormal(x, y);
    const double nx = normal.x;
    const double ny = normal.y;
    return sqrt(nx * nx + ny * ny) / normal.z;
}

double Terrain::getRawHeight(double x, double y) const {
    int size = getResolution();
    int posX = clamp(static_cast<int>(round(x * (size - 1))), 0, size - 1);
    int posY = clamp(static_cast<int>(round(y * (size - 1))), 0, size - 1);

    return _array(posX, posY);
}

double Terrain::getInterpolatedHeight(
    double x, double y, const Interpolation::interpFunc &func) const {
    int width = (int)(_array.n_rows - 1);
    int height = (int)(_array.n_cols - 1);

    x *= width;
    y *= height;
    int xi = clamp((int)floor(x), 0, width - 1);
    int yi = clamp((int)floor(y), 0, height - 1);

    double v1 = Interpolation::interpolate(xi, _array(xi, yi), xi + 1,
                                           _array(xi + 1, yi), x, func);
    double v2 = Interpolation::interpolate(xi, _array(xi, yi + 1), xi + 1,
                                           _array(xi + 1, yi + 1), x, func);
    return Interpolation::interpolate(yi, v1, yi + 1, v2, y, func);
}

double Terrain::getCubicHeight(double x, double y) const {
    double res = getResolution();
    x *= res;
    y *= res;
    int xi = static_cast<int>(floor(x));
    int yi = static_cast<int>(floor(y));
    
    double vx[4];
    for (int xn = 0; xn < 4; ++xn) {
        double vy[4];
        for (int yn = 0; yn < 4; ++yn) {
            vy[yn] = _array(clamp(xi + xn, 0, res - 1), clamp(yi + yn, 0, res - 1));
        }
        vx[xn] = cuberp(vy, y - yi);
    }
    return cuberp(vx, x - xi);
}

double Terrain::getExactHeightAt(double x, double y) const {
    int width = (int)(_array.n_rows - 1);
    int height = (int)(_array.n_cols - 1);

    x *= width;
    y *= height;
    int xi = clamp((int)floor(x), 0, width - 1);
    int yi = clamp((int)floor(y), 0, height - 1);

    // Interpolation to get the exact height of the point on the mesh
    // (triangular interpolation)
    // TODO method triangular interp ?
    double xd = x - xi;
    double yd = y - yi;
    double sumd = xd + yd;
    double a;

    if (sumd > 1) {
        double temp = xd;
        xd = 1 - yd;
        yd = 1 - temp;
        sumd = 2 - sumd;
        a = _array(xi + 1, yi + 1);
    } else {
        a = _array(xi, yi);
    }

    if (sumd <= std::numeric_limits<double>::epsilon()) {
        return a;
    }

    double b = _array(xi + 1, yi);
    double c = _array(xi, yi + 1);

    double ab = a * (1 - sumd) + b * sumd;
    double ac = a * (1 - sumd) + c * sumd;
    return ab * (xd / sumd) + ac * (yd / sumd);
}

double Terrain::getSlopeAt(double x, double y) const {
    int size = getResolution() - 1;
    int x0 = static_cast<int>(clamp(floor(x * size), 0, size - 1));
    int y0 = static_cast<int>(clamp(floor(y * size), 0, size - 1));

    double xd = x * size - x0;
    double yd = y * size - y0;

    double iy1 = (1 - xd) * getSlope(x0, y0) + xd * getSlope(x0 + 1, y0);
    double iy2 =
        (1 - xd) * getSlope(x0, y0 + 1) + xd * getSlope(x0 + 1, y0 + 1);
    return (1 - yd) * iy1 + yd * iy2;
}

Mesh *Terrain::createMesh() const {
    auto lower = _bbox.getLowerBound();
    auto size = _bbox.getUpperBound() - lower;
    return createMesh(lower.x, lower.y, lower.z, size.x, size.y, size.z);
}

Mesh *Terrain::createMesh(double sizeX, double sizeY, double sizeZ) const {
    return createMesh(-sizeX / 2, -sizeY / 2, 0, sizeX, sizeY, sizeZ);
}

Mesh *Terrain::createMesh(double offsetX, double offsetY, double offsetZ,
                          double sizeX, double sizeY, double sizeZ) const {
    Mesh *mesh = new Mesh();

    const int size = static_cast<int>(_array.n_rows);
    const int size_1 = size - 1;
    const double inv_size_1 = 1. / size_1;

    // Memory allocation
    int vertCount = size * size;
    mesh->reserveVertices(vertCount);

    // Vertices
    for (int y = 0; y < size; y++) {
        const double yd = y * inv_size_1;
        const double ypos = yd * sizeY + offsetY;

        for (int x = 0; x < size; x++) {
            const double xd = x * inv_size_1;
            const double xpos = xd * sizeX + offsetX;

            Vertex &vert = mesh->newVertex();

            vert.setPosition(xpos, ypos, _array(x, y) * sizeZ + offsetZ);
            vert.setTexture(xd, 1 - yd);

            // Compute normal
            double xUnit = sizeX * inv_size_1;
            double yUnit = sizeY * inv_size_1;
            vec3d nx{
                (_array(max(x - 1, 0), y) - _array(min(x + 1, size_1), y)) *
                    sizeZ,
                0, xUnit * 2};
            vec3d ny{
                0,
                (_array(x, max(y - 1, 0)) - _array(x, min(y + 1, size_1))) *
                    sizeZ,
                yUnit * 2};
            vert.setNormal((nx + ny).normalize());
        }
    }

    // Faces
    auto indice = [this](int x, int y) -> int {
        return y * this->_array.n_rows + x;
    };
    mesh->reserveFaces(size_1 * size_1 * 2);

    for (int y = 0; y < size_1; y++) {
        for (int x = 0; x < size_1; x++) {
            Face &face1 = mesh->newFace();
            Face &face2 = mesh->newFace();

            face1.setID(0, indice(x, y));
            face1.setID(1, indice(x + 1, y));
            face1.setID(2, indice(x, y + 1));

            face2.setID(0, indice(x + 1, y + 1));
            face2.setID(1, indice(x, y + 1));
            face2.setID(2, indice(x + 1, y));
        }
    }

    return mesh;
}

Image Terrain::createImage() const { return Image(this->_array); }

void Terrain::setTexture(const Image &image) { _texture = image; }

void Terrain::setTexture(Image &&image) { _texture = image; }

Image &Terrain::getTexture() { return _texture; }

const Image &Terrain::getTexture() const { return _texture; }

vec2i Terrain::getPixelPos(double x, double y) const {
    return {(int)min(x * _array.n_rows, _array.n_rows - 1),
            (int)min(y * _array.n_cols, _array.n_cols - 1)};
}

// -------

/*
 * I tried an interpolation based method, but it's not very efficient
 * in that case. Though, I'll need to implement trilinear interp, and this
 * a good example of what we can do.
 *
double Terrain::getSlopeAt(double x, double y) const {
    int size = getResolution() - 1;
    double unit = 1. / size;

    int x0 = static_cast<int>(clamp(floor(x * size - 1), 0, size - 2));
    int y0 = static_cast<int>(clamp(floor(y * size - 1), 0, size - 2));

    double xd = x * size - x0;
    double yd = y * size - y0;

    auto interp = [](double a0, double a1, double a2, double a) {
        return a0 + (a2 - a0) * a / 2;
        //return a * (a - 1) * 0.5 * a0 + (1 - a * a) * a1 + a * (a + 1) * 0.5 *
a2;
    };

    auto deriv = [](double a0, double a1, double a2, double a) {
        return a2 - a0;
        //return (a - 0.5) * a0 + (-2 * a) * a1 + (a + 0.5) * a2;
    };

    double iy[4];
    for (int i = 0; i < 3; ++i) {
        iy[i] = interp(_array(x0, y0 + i), _array(x0 + 1, y0 + i), _array(x0 +
2, y0 + i), xd);
    }
    double ix[4];
    for (int i = 0; i < 3; ++i) {
        ix[i] = interp(_array(x0 + i, y0), _array(x0 + i, y0 + 1), _array(x0 +
i, y0 + 2), yd);
    }

    double dx = deriv(iy[0], iy[1], iy[2], yd);
    double dy = deriv(ix[0], ix[1], ix[2], xd);
    return sqrt(dx * dx + dy * dy) * size;
}*/
} // namespace world
