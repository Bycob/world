#include "Terrain.h"

#include <algorithm>
#include <iostream>
#include <math.h>

#include <math/MathsHelper.h>

#include "assets/Interop.h"
#include "assets/Image.h"
#include "assets/Mesh.h"
#include "assets/MeshOps.h"

using namespace arma;

namespace world {

	Terrain::Terrain(int size) :
			_array(size, size),
			_texture(nullptr),
			_bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}) {

	}

	Terrain::Terrain(const Mat<double> &data) :
			_array(data),
			_texture(nullptr),
			_bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}) {

		if (data.n_rows != data.n_cols) {
			throw std::runtime_error("Terrain must be squared !");
		}
	}

	Terrain::Terrain(const Terrain &terrain)
			: _array(terrain._array),
			  _bbox(terrain._bbox) {

		if (terrain._texture != nullptr) {
			_texture = std::make_unique<Image>(*terrain._texture);
		}
	}

	Terrain::Terrain(Terrain &&terrain)
			: _array(std::move(terrain._array)),
			  _texture(std::move(terrain._texture)),
			  _bbox(terrain._bbox) {

	}

	Terrain::~Terrain() = default;

	Terrain &Terrain::operator=(const Terrain &terrain) {
		_array = terrain._array;
		_bbox = terrain._bbox;

		if (terrain._texture != nullptr) {
			_texture = std::make_unique<Image>(*terrain._texture);
		}

		return *this;
	}

	void Terrain::setBounds(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) {
		_bbox.reset({xmin, ymin, zmin}, {xmax, ymax, zmax});
	}

	const BoundingBox &Terrain::getBoundingBox() const {
		return _bbox;
	}

	double Terrain::getRawHeight(double x, double y) const {
        int size = getResolution();
		int posX = clamp(static_cast<int>(round(x * (size - 1))), 0, size - 1);
		int posY = clamp(static_cast<int>(round(y * (size - 1))), 0, size - 1);

		return _array(posX, posY);
	}

    double Terrain::getInterpolatedHeight(double x, double y, const Interpolation::interpFunc &func) const {
        int width = (int) (_array.n_rows - 1);
        int height = (int) (_array.n_cols - 1);

        x *= width;
        y *= height;
        int xi = clamp((int) floor(x), 0, width - 1);
        int yi = clamp((int) floor(y), 0, height - 1);

		double v1 = Interpolation::interpolate(
				xi, _array(xi, yi), xi + 1, _array(xi + 1, yi), x, func
		);
		double v2 = Interpolation::interpolate(
				xi, _array(xi, yi + 1), xi + 1, _array(xi + 1, yi + 1), x, func
		);
		return Interpolation::interpolate(yi, v1, yi + 1, v2, y, func);
    }

	double Terrain::getExactHeightAt(double x, double y) const {
		int width = (int) (_array.n_rows - 1);
		int height = (int) (_array.n_cols - 1);

		x *= width;
		y *= height;
		int xi = clamp((int) floor(x), 0, width - 1);
		int yi = clamp((int) floor(y), 0, height - 1);

		// Interpolation to get the exact height of the point on the mesh (triangular interpolation)
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
		}
		else {
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

	Mesh *Terrain::createMesh() const {
		auto lower = _bbox.getLowerBound();
		auto size = _bbox.getUpperBound() - lower;
		return createMesh(lower.x, lower.y, lower.z, size.x, size.y, size.z);
	}

	Mesh *Terrain::createMesh(double sizeX, double sizeY, double sizeZ) const {
		return createMesh(-sizeX / 2, -sizeY / 2, 0, sizeX, sizeY, sizeZ);
	}

	Mesh *Terrain::createMesh(double offsetX, double offsetY, double offsetZ, double sizeX, double sizeY,
							  double sizeZ) const {
		Mesh *mesh = new Mesh();

		const int size = static_cast<int>(_array.n_rows);
		const int size_1 = size - 1;
		const double inv_size_1 = 1. / size_1;

		// Memory allocation
		int vertCount = size * size;
		mesh->reserveVertices(vertCount);

		//Vertices
		for (int y = 0; y < size; y++) {
			const double yd = y * inv_size_1;
			const double ypos = yd * sizeY + offsetY;

			for (int x = 0; x < size; x++) {
				const double xd = x * inv_size_1;
				const double xpos = xd * sizeX + offsetX;

				Vertex &vert = mesh->newVertex();

				vert.setPosition(xpos, ypos, _array(x, y) * sizeZ + offsetZ);
				vert.setTexture(xd, 1 - yd);
			}
		}

		//Faces
		auto indice = [this](int x, int y) -> int { return y * this->_array.n_rows + x; };
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

		MeshOps::recalculateNormals(*mesh);

		return mesh;
	}

	Image Terrain::createImage() const {
		return Image(this->_array);
	}

	void Terrain::setTexture(const Image &image) {
		_texture = std::make_unique<Image>(image);
	}

	void Terrain::setTexture(Image &&image) {
		_texture = std::make_unique <Image>(image);
	}

	optional<const Image &> Terrain::getTexture() const {
		if (_texture == nullptr) {
			return nullopt;
		}
		return *_texture;
	}

	vec2i Terrain::getPixelPos(double x, double y) const {
		return {
				(int) min(x * _array.n_rows, _array.n_rows - 1),
				(int) min(y * _array.n_cols, _array.n_cols - 1)
		};
	}
}
